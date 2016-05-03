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

using namespace Eigen;
using namespace std;


int main(){
	//ARマーカーの位置の場所登録
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
		cv::Mat frame;
		cap >> frame; //get a new frame from camera
    	// カメラパラメタのロード
		aruco::CameraParameters params;
    	params.readFromXMLFile("./intrinsics.yml");
        params.resize(frame.size());
		// マーカを認識する
        aruco::MarkerDetector detector;
   		std::vector<aruco::Marker> markers;
    	const float markerSize = 0.05f; //マーカーサイズに合わせて変更する
    	detector.detect(frame, markers, params, markerSize);
    	// 結果を書き出す
    	auto outputImage = frame.clone();
  		vector<float> Y;
		vector<int>  ID;
		for (auto&& marker : markers) {
			ID.push_back(marker.id);
    		Y.push_back(marker.Tvec.at<float>(0,0)*(-1.0));
		    Y.push_back(marker.Tvec.at<float>(0,1));
		    Y.push_back(marker.Tvec.at<float>(0,2));
        	marker.draw(outputImage, cv::Scalar(0, 0, 255), 2);
        	aruco::CvDrawingUtils::draw3dCube(outputImage, marker, params);
    		}
		//vector→Vector変換
    	int N3=Y.size();
		if (N3 !=0){
			VectorXf y(N3);
			VectorXf id(int(ID.size()));
		int j=0;
		for (j;j<int(N3/3);j++){
			y(3*j)=Y[3*j];
			y(3*j+1)=Y[3*j+1];
			y(3*j+2)=Y[3*j+2];
			id(j)=ID[j];
		}
		//予測
		end = clock();
		t=double(end - start)/CLOCKS_PER_SEC;
		cout << "duration"<< t << endl;
		//速度の取得
		u(0,0)=0.0;
		u(1,0)=0.0;
		u(2,0)=0.0;
		//u(0,0)=px_selfstate.vision_vx;
		//u(1,0)=px_selfstate.vision_vy;
		//u(2,0)=px_selfstate.vision_vz	;
		mu_=F*mu+G*u*t; //tは取得時間間隔
		Sigma_=Q+F*Sigma*F.transpose();
		start =clock();
		//更新
		//H　観測行列 3N×3, R 観測ノイズの共分散行列　3N×3N
		VectorXf 	yi(N3);
		VectorXf  yres(N3);
		MatrixXf	H(N3,3);
		MatrixXf R = MatrixXf::Identity(N3,N3);
		MatrixXf  S(N3,N3);
		MatrixXf	K(3,N3);
		int i=0;
		for (i;i<(N3/3);i++){
			H(3*i,0)=1;
			H(3*i,1)=0;
			H(3*i,2)=0;
			H(3*i+1,0)=0;
			H(3*i+1,1)=1;
			H(3*i+1,2)=0;
			H(3*i+2,0)=0;
			H(3*i+2,1)=0;
			H(3*i+2,2)=1;
			yres(3*i)  =(AR_id[id(i)])[0];
			yres(3*i+1)= (AR_id[id(i)])[1];
			yres(3*i+2)=(AR_id[id(i)])[2];
		}
		yi=y-H*mu_+yres;
		S=H*Sigma_*H.transpose()+R;
		K=Sigma_*H.transpose()*S.inverse();
		mu=mu_+K*yi;
		Sigma=Sigma_-K*H*Sigma_;
		cout << mu << endl;
	}
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
