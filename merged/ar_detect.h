//ar_detect.h
#ifndef __AR_DETECT_H_INCLUDED__
#define __AR_DETECT_H_INCLUDED__
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

class AR_DETECT{
  public:
    map<int,Vector3f> AR_id;
  	//定数
  	double t;
  	int N3;
  	clock_t start;
  	clock_t end;
  	Vector3f zero ;
  	Matrix3f F,G,Q;
    //変数
  	Vector3f u,mu,mu_;
  	Matrix3f Sigma,Sigma_;
    VectorXf y;
    CameraParameters params;
  	MarkerDetector detector;
  	vector<Marker> markers;

    AR_DETECT();  //コンストラクタ
  	CameraParameters CameraLoad(Mat frame); //カメラロード
  	vector<Marker> ARDetect(Mat frame, CameraParameters params);//マーカーを認識する
  	Mat outPut(Mat frame);
    Vector3f LKF(Mat outputImage,vector<Marker> markers,CameraParameters params,map<int,Vector3f> AR_id,Vector3f u);
    };

#endif
