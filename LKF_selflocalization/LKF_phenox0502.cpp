#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <aruco.h>
#include <vector>
#include <time.h>
#include "ar_detect.h"
using namespace Eigen;
using namespace std;
using namespace cv;
using namespace aruco;

map<int,Vector3f> AR_id;

int main(){
	AR_DETECT ad;
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
  cv::VideoCapture cap(1);//デバイスのオープン
   if(!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
  {
    return -1;
      }
	start =0.0;
  while(1)
    {
			Mat frame;
			cap >> frame; //get a new frame from camera
			CameraParameters params = ad.CameraLoad(frame);
			vector<Marker> markers  = ad.ARDetect(frame,params);
	  	// 結果を書き出す
			Mat outputImage =ad.outPut(frame);
			ad.LKF(outputImage,markers,params,AR_id);
			// 結果を書き出す
			cv::namedWindow("output");
			cv::imshow("output", outputImage);
			// 何故か入れないとループが回らない……
			int key = cv::waitKey(1);
				if(key == 113)//qボタンが押されたとき
				{
		    	break;//whileループから抜ける．
				}
			}
	return 0;
}
