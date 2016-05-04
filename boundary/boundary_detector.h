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

    double blue_r;
    double blue_g;
    double blue_b;
    double blue_th;

    double green_r;
    double green_g;
    double green_b;
    double green_th;

    int efilter[30];

    int area_thresh_;
    int dmin_thresh_;
    int skip_step_;

    BoundaryDetector(); 
    void extract_rgb(Mat* src, Mat* dst, double cr, double cg, double cb, double th);
    void detect_edge(Mat* in, Mat* out, Mat* edge);
    bool calc_g(Mat *src, Vector2f *g);
    int findLine(Mat *src, Vector2f g, Vector2f *norm_start1, Vector2f *norm1, Vector2f *norm_start2, Vector2f *norm2);
    int get_norm(Mat *org, Vector2f *norm_start1, Vector2f *norm1, Vector2f *norm_start2, Vector2f *norm2);
    
};

#endif 
