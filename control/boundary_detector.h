#ifndef  BOUNDARY_
#define  BOUNDARY_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <Eigen/Core>

using namespace Eigen;
using namespace cv;

class BoundaryDetector{
public:
    // parameter for extract color
    double blue_r;
    double blue_g;
    double blue_b;
    double blue_th;

    double yellow_r;
    double yellow_g;
    double yellow_b;
    double yellow_th;

    // filter for detect_edge
    int efilter[30];

    // number of pixels used to detect edge
    int edge_num_;
    // how many pixels to skip in extract_rgb
    int skip_step_;


    BoundaryDetector(); 
    // use this function to get norm
    int get_norm(Mat *org, Vector2f *norm_start1, Vector2f *norm1, Vector2f *norm_start2, Vector2f *norm2);

    // extract one color from rgb image
    void extract_rgb(Mat* src, Mat* dst, double cr, double cg, double cb, double th);
    // calculate edge between two color
    void detect_edge(Mat* in, Mat* out, Mat* edge);
    // calculate center of gravity from binary image
    bool calc_g(Mat *src, Vector2f *g, Vector2f center, int range);
    // calculate the number of outside color near to the norm direction
    int count_direction(Mat *out, Vector2f center, Vector2f norm, int range);
    // use HoughLineP and kmeans to detect two lines from edge image
    int findLine(Mat *src, Mat *in1_image, Vector2f *norm_start1, Vector2f *norm1, Vector2f *norm_start2, Vector2f *norm2);
    
};

#endif 
