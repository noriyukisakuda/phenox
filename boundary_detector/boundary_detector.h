#ifndef  BOUNDARY_
#define  BOUNDARY_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <Eigen/Core>

using namespace Eigen;
using namespace cv;

class BoundaryDetector{
public:
    // parameter
    int green_h_low_;
    int green_h_high_;
    int green_s_;
    int green_v_;

    int red_h_low_;
    int red_h_high_;
    int red_s_;
    int red_v_;

    int area_thresh_;
    int dmin_thresh_;

    BoundaryDetector(); 
    void extract_color(Mat* src, Mat* dst, int h_th_low, int h_th_up, int s_th, int v_th);
    static bool compareContourAreas(vector<Point> contour1, vector<Point> contour2);
    bool approx_rect(Mat *img, vector<Point> *approx);
    bool calc_norm(vector<Point> points_in, vector<Point> points_out, Vector2f *norm_start, Vector2f *norm);
    bool get_norm(Mat * org, Vector2f *norm_start, Vector2f *norm);
    
};

#endif 
