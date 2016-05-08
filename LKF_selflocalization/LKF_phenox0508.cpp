//phenox part
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/time.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <fstream>
#include <turbojpeg.h>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "pxlib.h"
#include "parameter.h"

//AR part
#include "ar_detect.h"
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <aruco.h>
#include <vector>
#include <time.h>

using namespace Eigen;
using namespace std;
using namespace cv;
using namespace aruco;


static char timer_disable = 0;

//function declaration
static void timerhandler(int i);
static void setup_timer();

//variable declaration
const px_cameraid cameraid = PX_FRONT_CAM;
const int ftmax = 200;
const int imgcount_max = 10;
const float recordtime = 5.0;
px_selfstate st;

//AR pos declaration
map<int,Vector3f> AR_id;

int main(int argc, char **argv)
{
  int i;
  //Initial Settings
  pxinit_chain();
  set_parameter();
  printf("CPU0:Start Initialization. Please do not move Phenox.\n");
  while(!pxget_cpu1ready());
  setup_timer();
  printf("CPU0:Finished Initialization.\n");

  //Image processing starts//
  printf("CPU0:Image processing Example.\n");

  //Image Initialization
  Mat frame;
  IplImage *testImage = cvCreateImage(cvSize(320,240), IPL_DEPTH_8S, 3);
  //IplImage *srcImage,*copyImage;
  //copyImage = cvCreateImage(cvSize(320,240),IPL_DEPTH_8U,3);
  px_imgfeature *ft =(px_imgfeature *)calloc(ftmax,sizeof(px_imgfeature));
  int ftstate = 0;
  int ftnum = 0;

  int imgcount = 0;

  cout<< 1 <<endl;
  cout<< 2 <<endl;
  //ARマーカーの位置の場所登録随時追加
  map<int,Vector3f> AR_id;
  Vector3f v0(-0.18,0.18,0.0);
  Vector3f v10(0.18,0.18,0.0);
  Vector3f v20(-0.18,-0.18,0.0);
  Vector3f v30(0.18,-0.18,0.0);
  AR_id[0] =  v0;
  AR_id[10]=  v10;
  AR_id[20]=  v20;
  AR_id[30]=  v30;

  //定数
  double t;
  clock_t start;
  clock_t end;
  Vector3f zero ;
  Matrix3f F,G,Q;
  zero = Vector3f::Zero();
  F  = Matrix3f::Identity();
  G  = Matrix3f::Identity();
  Q  = Matrix3f::Identity();
  //変数
  Vector3f u,mu,mu_;
  Matrix3f Sigma,Sigma_;
  //Sigma　共分散行列
  Sigma= Matrix3f::Zero();
  //観測情報の取得
  cout<<3<<endl;
  start =0.0;
  cout<<4<<endl;
  AR_DETECT *ad;
  ad = new AR_DETECT;
  delete ad;
  while(1){
      if(pxget_imgfullwcheck(cameraid,&testImage) == 1) {
        frame=cvarrToMat(testImage);
        //カメラのパラメーター設定
        CameraParameters params = ad->CameraLoad(frame);
        //マーカーを認識
        vector<Marker> markers  = ad->ARDetect(frame,params);
        //認識結果を書き出す
        Mat outputImage =ad->outPut(frame);
        //位置を推定する
        ad->LKF(outputImage,markers,params,AR_id);
        // 結果を出力する（画像
        //cv::namedWindow("output");
        //cv::imshow("output", outputImage);
        // 何故か入れないとループが回らない……
        int key = cv::waitKey(1);
          if(key == 113)//qボタンが押されたとき
          {
            break;//whileループから抜ける．
          }
        }
    cout<<7<<endl;
    //pxget_selfstate(&st);
    //cout<<st.vision_vx<<endl;
    usleep(100000);
  } 

  return 0;
}



static void setup_timer() {
  struct sigaction action;
  struct itimerval timer;

  memset(&action, 0, sizeof(action));

  action.sa_handler = timerhandler;
  action.sa_flags = SA_RESTART;
  sigemptyset(&action.sa_mask);
  if(sigaction(SIGALRM, &action, NULL) < 0){
    perror("sigaction error");
    exit(1);
  }

  /* set interval timer (10ms) */
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 10000;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 10000;
  if(setitimer(ITIMER_REAL, &timer, NULL) < 0){
    perror("setitimer error");
    exit(1);
  }
}

void timerhandler(int i) {
  char c;

  if(timer_disable == 1) {
    return;
  }

  pxset_keepalive();
  pxset_systemlog();

  px_selfstate st;
  pxget_selfstate(&st);

  pxset_img_seq(cameraid);

  static unsigned long msec_cnt = 0;
  msec_cnt++;
  if(!(msec_cnt % 100)){
    //printf("%.2f %.2f %.2f | %.2f %.2f %.2f | %.2f\n",st.degx,st.degy,st.degz,st.vision_tx,st.vision_ty,st.vision_tz,st.height);
  }

  if(pxget_battery() == 1) {
    timer_disable = 1;
    system("shutdown -h now\n");
    exit(1);
  }

  return;
}


