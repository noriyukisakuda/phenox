// #include <opencv2/opencv.hpp>
// #include <iostream>
// #include <Eigen/Core>
#include "boundary_detector.h"

using namespace std;
using namespace cv;
using namespace Eigen;

int main(int argc, char *argv[]){
    //カメラの設定
    VideoCapture cap(0);
    if(!cap.isOpened()){
        cout << "Camera not Found!" << endl;
        return -1;
    }

    //画像を入れるMatの宣言
    Mat org;    //カメラ画像
    Mat red_image, green_image;
    Vector2f norm, norm_start, norm_end;

    BoundaryDetector bd;
    
    while(waitKey(1)%256 != 'q'){
        cap >> org;

        if(bd.get_norm(&org, &norm_start, &norm)){
            norm_end = norm_start + 100 * norm;
            line(org, Point(norm_start.x(), norm_start.y()), Point(norm_end.x(), norm_end.y()), Scalar(0, 255, 255), 3, 4 );
        }

        imshow("Original", org);
    }
}
