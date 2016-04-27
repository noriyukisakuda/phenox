#include <opencv2/opencv.hpp>
#include <iostream>
#include <Eigen/Core>
#include "boundary_detector.h"

using namespace std;
using namespace cv;
using namespace Eigen;

BoundaryDetector::BoundaryDetector(){
    green_h_low_ = 20;
    green_h_high_ = 100;
    green_s_ = 60;
    green_v_ = 30;

    red_h_low_ = 270;
    red_h_high_ = 15;
    red_s_ = 60;
    red_v_ = 20;

    area_thresh_ = 500;
    dmin_thresh_ = 40;
}

void BoundaryDetector::extract_color(Mat* src, Mat* dst, int h_th_low, int h_th_up, int s_th, int v_th){
    Mat hsv;
    cvtColor(*src, hsv, COLOR_BGR2HSV);
    vector<Mat> planes;
    split(hsv, planes);
    Mat h_dst_1, h_dst_2;
    Mat s_dst, v_dst;

    if(h_th_low > h_th_up){
        threshold(planes[0], h_dst_1, h_th_low, 255, THRESH_BINARY);
        threshold(planes[0], h_dst_2, h_th_up, 255, THRESH_BINARY_INV);
        bitwise_or(h_dst_1, h_dst_2, *dst);
    }
    else{
        threshold(planes[0], *dst, h_th_low, 255, THRESH_TOZERO);
        threshold(*dst, *dst, h_th_up, 255, THRESH_TOZERO_INV);
        threshold(*dst, *dst, 0, 255, THRESH_BINARY);
    }
    threshold(planes[1], s_dst, s_th, 255, THRESH_BINARY);
    threshold(planes[2], v_dst, v_th, 255, THRESH_BINARY);

    bitwise_and(*dst, s_dst, *dst);
    bitwise_and(*dst, v_dst, *dst);

    return;
}

// comparison function object
bool BoundaryDetector::compareContourAreas ( vector<Point> contour1, vector<Point> contour2 ) {
    double i = fabs( contourArea(Mat(contour1)) );
    double j = fabs( contourArea(Mat(contour2)) );
    return ( i < j );
}

bool BoundaryDetector::approx_rect(Mat *img, vector<Point> *approx){

    erode(*img, *img, Mat(), Point(-1,-1), 1);
    dilate(*img, *img, Mat(), Point(-1,-1), 1);
    vector<vector<Point> > contours;
    findContours(*img, contours, CV_RETR_TREE, CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(), compareContourAreas);

    unsigned int poly_corner = 4;

    for(unsigned int i = 0; i < contours.size(); i++){
        double arclen = arcLength(Mat(contours[i]), true);
        approxPolyDP(Mat(contours[i]), *approx, 0.02*arclen, true);
        if(approx->size() == poly_corner){
            if(contourArea(Mat(contours[i])) > area_thresh_)
                return true;
        }
    }
    return false;
}
    
bool BoundaryDetector::calc_norm(vector<Point> points_in, vector<Point> points_out, Vector2f *norm_start, Vector2f *norm){
    Vector2f g(0, 0);
    for(int i = 0; i < 4; i++){
        g.x() += 0.25 * points_out[i].x;
        g.y() += 0.25 * points_out[i].y;
    }
    int min_in = 0;
    int min_out = 0;
    double dminj = 1000;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            double dx = points_in[i].x - points_out[j].x;
            double dy = points_in[i].y - points_out[j].y;
            double d = sqrt(dx * dx + dy * dy);
            if(d < dminj){
                dminj = d;
                min_in = i;
                min_out = j;
            }
        }
    }
    Vector2f v_in, v_out, v_in_v, v_start;
    v_start.x() = 0.5 * (points_in[min_in].x + points_out[min_out].x);
    v_start.y() = 0.5 * (points_in[min_in].y + points_out[min_out].y);
    int max_in = 0;
    double dmin = 1000;
    for(int i = 0; i < 4; i++){
        if(i != min_in){
            v_in << points_in[i].x - v_start.x(), points_in[i].y - v_start.y();
            v_in_v << v_in.y(), -v_in.x();
            v_in_v /= v_in_v.norm();
            for(int j = 0; j < 4; j++){
                if(j != min_out){
                    v_out << points_out[j].x - v_start.x(), points_out[j].y - v_start.y();
                    double d = fabs(v_in_v.dot(v_out));
                    if(d < dmin && v_in.dot(v_out) > 0){
                        dmin = d;
                        max_in = i;
                    }
                }
            }
        }
    }
    Vector2f v_end;
    if(dmin < dmin_thresh_){
        v_end.x() = points_in[max_in].x;
        v_end.y() = points_in[max_in].y;
        Vector2f v_line;
        v_line = v_end - v_start;
        Vector2f norm_, vecg;
        *norm_start = 0.5 * v_start + 0.5 * v_end;
        vecg = g - v_start;
        norm_ << v_line.y(), -v_line.x();
        norm_ /= norm_.norm();
        norm_ = norm_.dot(vecg) * norm_;
        norm_ /= norm_.norm();
        *norm = norm_;
        return true;
    }
    return false;
}

bool BoundaryDetector::get_norm(Mat *org, Vector2f *norm_start, Vector2f *norm){

    Mat red_image, green_image;
    
    extract_color(org, &red_image, red_h_low_, red_h_high_, red_s_, red_v_);
    extract_color(org, &green_image, green_h_low_, green_h_high_, green_s_, green_v_);
    vector<Point> approx_red, approx_green;
    bool apprect_red = approx_rect(&red_image, &approx_red);
    bool apprect_green = approx_rect(&green_image, &approx_green);
    if(apprect_red && apprect_green){
        // Vector2f norm, norm_start, norm_end;
        bool calc = calc_norm(approx_red, approx_green, norm_start, norm);
        return calc;
    }
    return false;
}

