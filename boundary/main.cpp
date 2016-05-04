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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
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
#include <turbojpeg.h>
#include "boundary_detector.h"


using namespace std;
using namespace cv;
using namespace Eigen;

static void setup_timer();

static char timer_disable = 0;
static void *timer_handler(void *ptr);

pthread_t timer_thread;

// const px_cameraid cameraid = PX_FRONT_CAM;
const px_cameraid cameraid = PX_BOTTOM_CAM;


double get_time() {
    struct timeval  tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; 
}

int main(int argc, char **argv)
{
  int i,j,count;
  count = 0;

  int client_sockfd;
  int len ;
  struct sockaddr_un address;
  int result ;
  client_sockfd = socket(AF_UNIX,SOCK_STREAM,0);
  address.sun_family = AF_UNIX ;
  strcpy(address.sun_path , "/root/nodejs/projects/imgserver/mysocket");
  len = sizeof(address);
  result = connect(client_sockfd , (struct sockaddr *)&address , len);
  if(result != 0) {exit(-1);}


  pxinit_chain();
  set_parameter();   
  printf("CPU0:Start Initialization. Please do not move Phenox.\n");
  while(!pxget_cpu1ready());
  setup_timer();
  printf("CPU0:Finished Initialization.\n");
  
  // CvMat *mat;
  Mat mat;

  int param[]={CV_IMWRITE_JPEG_QUALITY,70};
  IplImage *testImage;    
  count = 0;

  pxset_led(0,1); 
  const int ftmax = 200;
  px_imgfeature *ft =(px_imgfeature *)calloc(ftmax,sizeof(px_imgfeature));
  int ftstate = 0;

  long unsigned int buffer_size = 128000;
  unsigned char compressed_buffer_memory[128000];
  unsigned char *compressed_buffer = (unsigned char*)compressed_buffer_memory;
  double delta, start_time;
  delta = start_time = get_time();
  int frames_count = 0;
  double fps = 0.0;
  double last_time = get_time();
  BoundaryDetector bd;
  Vector2f norm, norm_start, norm_end, norm2, norm_start2, norm_end2;
  norm_start << 0, 0;
  norm << 0, 0;
  int boundary_cnt = 0;
  while(1) {         
    if(pxget_imgfullwcheck(cameraid,&testImage) == 1) {	
      frames_count++;
      cout << frames_count << endl;
      mat = cvarrToMat(testImage);
      int gn = bd.get_norm(&mat, &norm_start, &norm, &norm_start2, &norm2);
      cout << "get norm = " << gn << endl;
      cout << "norm_start" << norm_start << "\n normstart2" << norm_start2 << endl;
    if(gn == 1){
        norm_end = norm_start + 100 * norm;
        cvLine(testImage, cvPoint(norm_start.x(), norm_start.y()), cvPoint(norm_end.x(), norm_end.y()), CV_RGB(0, 255, 255), 3, 4 );
        boundary_cnt = 0;
    }
    else if(gn == 2){
        norm_end = norm_start + 100 * norm;
        norm_end2 = norm_start2 + 100 * norm2;
        cvLine(testImage, cvPoint(norm_start.x(), norm_start.y()), cvPoint(norm_end.x(), norm_end.y()), CV_RGB(0, 255, 255), 3, 4 );
        cvLine(testImage, cvPoint(norm_start2.x(), norm_start2.y()), cvPoint(norm_end2.x(), norm_end2.y()), CV_RGB(0, 255, 255), 3, 4 );
        boundary_cnt = 0;
    }
      // else if(boundary_cnt < 50){
      //     double vtfx = 0;
      //     double vtfy = 0;
      //     if(ftstate == 1) {
      //       int ftnum = pxget_imgfeature(ft,ftmax);
      //             if(ftnum >= 0) {
      //               for(i = 0;i < ftnum;i++) {
      //                 cvCircle(testImage,cvPoint((int)ft[i].pcx,(int)ft[i].pcy),2,CV_RGB(255,255,0),1,8,0);
      //                 cvCircle(testImage,cvPoint((int)ft[i].cx,(int)ft[i].cy),2,CV_RGB(0,255,0),1,8,0);
      //                 cvLine(testImage,cvPoint((int)ft[i].pcx,(int)ft[i].pcy),cvPoint((int)ft[i].cx,(int)ft[i].cy),CV_RGB(0,0,255),1,8,0);
      //                 vtfx += (int)ft[i].cx - (int)ft[i].pcx;
      //                 vtfy += (int)ft[i].cy - (int)ft[i].pcy;
      //               }
      //               vtfx /= 1.0 * ftnum;
      //               vtfy /= 1.0 * ftnum;
      //               ftstate = 0;
      //             }
      //     }
      //     norm_start.x() += vtfx;
      //     norm_start.y() += vtfy;
      //     norm_end = norm_start + 100 * norm;
      //     boundary_cnt++;
      // }
      // else{
      //     norm_start << 0, 0;
      //     norm << 0, 0;
      //     norm_end << 0, 0;
      // }
      // cvLine(testImage, cvPoint(norm_start.x(), norm_start.y()), cvPoint(norm_end.x(), norm_end.y()), CV_RGB(0, 255, 255), 3, 4 );
      if(pxset_imgfeature_query(cameraid) == 1) {
	      ftstate = 1;
      }

      tjhandle tj_compressor = tjInitCompress();
      buffer_size = 128000;
      unsigned char *buffer = (unsigned char*)testImage->imageData;
      // unsigned char *buffer = (unsigned char*)mat.data;
      
      double encoding_time = get_time();
      if (tjCompress2(tj_compressor, buffer, 320, 0, 240, TJPF_BGR,
                &compressed_buffer, &buffer_size, TJSAMP_420, 30,
                TJFLAG_NOREALLOC) == -1) {
          printf("%s\n", tjGetErrorStr());
      } else {
          encoding_time = get_time() - encoding_time;
          write(client_sockfd, compressed_buffer, buffer_size);
          count++;
      }
      // if (count % 30 == 0)
      //       printf("%f: Image encoded, index: %d size: %ld\n\t fps: %f delta: %f, encoding_time: %f\n", (get_time() - start_time)/1000.0,count++,buffer_size, fps, get_time() - delta, encoding_time);
      
      tjDestroy(tj_compressor);
      delta = get_time();
      
      if (get_time() - last_time > 3000.0) {
          fps = frames_count * 1000.0/(get_time() - last_time);
          last_time = get_time();
          frames_count = 0;
      }
    }             
  }
  usleep(2000);

}

static void setup_timer() {
    struct sigaction action;
    struct itimerval timer;
    timer_thread = pthread_create(&timer_thread, NULL, timer_handler, NULL);
}

void *timer_handler(void *ptr) {
    if(timer_disable == 1) {
        return NULL;
    }
    struct timespec _t;
    static struct timeval now, prev;
    double dt = 0;
    clock_gettime(CLOCK_REALTIME, &_t);
    while(1) {
        pxset_keepalive();
        pxset_systemlog();
        pxset_img_seq(cameraid);  

        static unsigned long msec_cnt = 0;
        msec_cnt++;

        gettimeofday(&now, NULL);
        dt = (now.tv_sec - prev.tv_sec) + 
                (now.tv_usec - prev.tv_usec) * 1.0E-6;
        // cout << dt << endl;
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

        // if(!(msec_cnt % 30)){
        //     printf("%.2f %.2f %.2f | %.2f %.2f %.2f | %.2f | \n",st.degx,st.degy,st.degz,st.vision_tx,st.vision_ty,st.vision_tz,st.height);
        // } 

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

        struct timespec remains;
        _t.tv_nsec += 10000000;
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &_t, &remains);
//        while (get_time() - t < 10.0) {
//            usleep(500);
//        }
        clock_gettime(CLOCK_REALTIME, &_t);
    }
}
