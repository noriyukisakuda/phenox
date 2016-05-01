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

    BoundaryDetector(); 
    void extract_color(Mat* src, Mat* dst, int h_th_low, int h_th_up, int s_th, int v_th);
    void extract_rgb(Mat* src, Mat* dst, double cr, double cg, double cb, double th);
    void detect_edge(Mat* in, Mat* out, Mat* edge);
    bool calc_g(Mat *src, Vector2f *g);
    void findLine(Mat *src, Vector2f vecg, Vector2f *norm_start, Vector2f *norm);
    static bool compareContourAreas(vector<Point> contour1, vector<Point> contour2);
    bool approx_rect(Mat *img, vector<Point> *approx);
    bool calc_norm(vector<Point> points_in, vector<Point> points_out, Vector2f *norm_start, Vector2f *norm);
    bool get_norm(Mat * org, Vector2f *norm_start, Vector2f *norm);
    
};

#endif 
