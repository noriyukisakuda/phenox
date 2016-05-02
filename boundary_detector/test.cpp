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
    // Mat blue_image, yellow_image;
    // Vector2f norm, norm_start, norm_end;
    // Vector2f norm2, norm_start2, norm_end2;
    Vector2f norm(0, 0);
    Vector2f norm_start(0, 0);
    Vector2f norm_end(0, 0);
    Vector2f norm2(0, 0);
    Vector2f norm_start2(0, 0);
    Vector2f norm_end2(0, 0);

    // cout << "read image.jpg" << endl;
    BoundaryDetector bd;
    
    while(waitKey(1)%256 != 'q'){
        cap >> org;
    
        if(bd.get_norm(&org, &norm_start, &norm, &norm_start2, &norm2) == 1){
            norm_end = norm_start + 100 * norm;
            line(org, Point(norm_start.x(), norm_start.y()), Point(norm_end.x(), norm_end.y()), Scalar(0, 255, 255), 3, 4 );
        // circle(org, Point(norm_start.x(), norm_start.y()), 15, Scalar(255, 0, 0), 3, 8, 0);
        }
        else if(bd.get_norm(&org, &norm_start, &norm, &norm_start2, &norm2) == 2){
            norm_end = norm_start + 100 * norm;
            norm_end2 = norm_start2 + 100 * norm2;
            line(org, Point(norm_start.x(), norm_start.y()), Point(norm_end.x(), norm_end.y()), Scalar(0, 255, 255), 3, 4 );
            line(org, Point(norm_start2.x(), norm_start2.y()), Point(norm_end2.x(), norm_end2.y()), Scalar(0, 255, 255), 3, 4 );
        // circle(org, Point(norm_start.x(), norm_start.y()), 15, Scalar(255, 0, 0), 3, 8, 0);
        // circle(org, Point(norm_start2.x(), norm_start.y()), 15, Scalar(255, 0, 0), 3, 8, 0);
        }
    
        imshow("Original", org);
    }
}
