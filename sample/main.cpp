/*
Copyright (c) 2015 Ryo Konomura

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <termios.h>
#include <pthread.h>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "pxlib.h"
#include "parameter.h"
#include <iostream>
#include <fstream>

using namespace std;

#define PI 3.14159265

static char timer_disable = 0;

static void *timer_handler(void *ptr);

pthread_t timer_thread;
static void setup_timer();
static int ftnum = 0;
const int ftmax = 200;
const int timer_usec = 10000;

const double RAD2DEG = 180 / PI;
const double DEG2RAD = PI / 180;

const double msec = 1;

const px_cameraid cameraid = PX_BOTTOM_CAM;

double get_msec() {
    struct timeval  tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0 ; 
}

int main(int argc, char **argv)
{
  pxinit_chain();
  set_parameter();
   
  printf("CPU0:Start Initialization. Please do not move Phenox.\n");
  while(!pxget_cpu1ready());
  setup_timer();
  printf("CPU0:Finished Initialization.\n");

  px_imgfeature *ft =(px_imgfeature *)calloc(ftmax,sizeof(px_imgfeature));
  int ftstate = 0;

  while(1) {
    if(ftstate == 0) {
      if(pxset_imgfeature_query(cameraid) == 1)
	ftstate = 1;
    }
    else if(ftstate == 1) {
      int res = pxget_imgfeature(ft,ftmax);
      if(res >= 0) {
	ftnum = res;
	ftstate = 0;      
      }
    }
    usleep(1000);
  }
}

static void setup_timer() {
    struct sigaction action;
    struct itimerval timer;
    timer_thread = pthread_create(&timer_thread, NULL, timer_handler, NULL);
}

void *timer_handler(void *ptr) {
    char c;  

    if(timer_disable == 1) {
        return NULL;
    }

    struct timespec _t;
    static struct timeval now, prev;
    double dt = 0;

    while(1){
        double _time_start = get_msec();
        clock_gettime(CLOCK_REALTIME, &_t);
        pxset_keepalive();
        pxset_systemlog();

        gettimeofday(&now, NULL);
        dt = (now.tv_sec - prev.tv_sec) + 
                (now.tv_usec - prev.tv_usec) * 1.0E-6;
        if(dt < 0){
            cout << "dt < 0" << endl;
            continue;
        }
        prev = now;

        // vision control
        double setdegx, setdegy;
        px_selfstate st;
        pxget_selfstate(&st);
        
        double kpx = 0.05;
        double kpy = 0.05;
        double kdx = 0.3;
        double kdy = 0.3;
        double kix = 0.000003;
        double kiy = 0.000003;

        static double txi = 0;
        static double tyi = 0;
        txi += st.vision_tx;
        tyi += st.vision_ty;
        double uy = -kpx * st.vision_tx - kdx * st.vision_vx - kix * txi;
        double ux = -kpy * st.vision_ty - kdy * st.vision_vy - kiy * tyi;
        ux = -ux;
        if(fabs(ux) > 50){
            ux = 50 * ux / fabs(ux);
        }
        if(fabs(uy) > 50){
            uy = 50 * uy / fabs(uy);
        }

        // save log
        static ofstream ofs_deg("output_deg");
            ofs_deg << st.degx << "," << st.degy << endl;
        static ofstream ofs_ctl("output_ctl");
            ofs_ctl << setdegx << "," << setdegy << endl;
        static ofstream ofs_vision("output_vision");
            ofs_vision << st.vision_tx << "," << st.vision_ty << "," << ux << "," << uy << endl;


        static int hover_cnt = 0;
        if(pxget_operate_mode() == PX_UP){
            pxset_visualselfposition(0, 0);
            txi = 0;
            tyi = 0;
            hover_cnt = 0;
        }

        if(pxget_operate_mode() == PX_HOVER) {
                if(hover_cnt < 100){
                    hover_cnt++;
                }
                else{
                    pxset_dst_degx(setdegx);
                    pxset_dst_degy(setdegy);
                }
        }

        static int prev_operatemode = PX_HALT;
        if((prev_operatemode == PX_UP) && (pxget_operate_mode() == PX_HOVER)) {
            pxset_visioncontrol_xy(st.vision_tx,st.vision_ty);
        }
        prev_operatemode = pxget_operate_mode();  

        if(pxget_whisle_detect() == 1) {
            if(pxget_operate_mode() == PX_HOVER) {
                pxset_operate_mode(PX_DOWN);
            }      
            else if(pxget_operate_mode() == PX_HALT) {
                pxset_rangecontrol_z(150);
                pxset_operate_mode(PX_UP);		   
            }      
        }
        if(pxget_battery() == 1) {
            timer_disable = 1;
            system("shutdown -h now\n");   
            exit(1);
        }
        double _time_end = get_msec();
        double sleep_time = msec - (_time_end - _time_start);
        while((get_msec() - _time_start) < msec);
    }
}

