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
    double blue_r;
    double blue_g;
    double blue_b;
    double blue_th;

    double yellow_r;
    double yellow_g;
    double yellow_b;
    double yellow_th;

    int efilter[30];

    int area_thresh_;
    int dmin_thresh_;
    int edge_num_;
    int skip_step_;

    BoundaryDetector(); 
    void extract_rgb(Mat* src, Mat* dst, double cr, double cg, double cb, double th);
    void detect_edge(Mat* in, Mat* out, Mat* edge);
    bool calc_g(Mat *src, Vector2f *g);
    int findLine(Mat *src, Vector2f g, Vector2f *norm_start1, Vector2f *norm1, Vector2f *norm_start2, Vector2f *norm2);
    int get_norm(Mat *org, Vector2f *norm_start1, Vector2f *norm1, Vector2f *norm_start2, Vector2f *norm2);
    
};

#endif 
