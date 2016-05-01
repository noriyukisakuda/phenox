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

    blue_r = 0.77;
    blue_g = -0.12;
    blue_b = -0.63;
    blue_th = 35;

    green_r = -0.46;
    green_g =  0.81;
    green_b = -0.34;
    green_th = 15;

    for(int i = 0;i < 30; i++)
        efilter[i] = 1;

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

void BoundaryDetector::extract_rgb(Mat* src, Mat* dst, double cr, double cg, double cb, double th){

    for(int y = 0; y < src->rows; y++){
            for(int x = 0; x < src->cols; x++){
                    // 画像のチャネル数分だけループ。白黒の場合は1回、カラーの場合は3回　　　　　
                    double r = src->data[ y * src->step + x * src->elemSize()];
                    double g = src->data[ y * src->step + x * src->elemSize() + 1];
                    double b = src->data[ y * src->step + x * src->elemSize() + 2];
                    double gray = r * cr + g * cg + b * cb;
                    if(gray > th){
                        dst->data[ y * dst->step + x * dst->elemSize()] = 2;
                    }
                    else{
                        dst->data[ y * dst->step + x * dst->elemSize()] = 0;
                    }
            }
    }
    return;
}

void BoundaryDetector::detect_edge(Mat* in, Mat* out, Mat* edge){
    int num = 25;
    for(int y = 2; y < in->rows - num; y++){
            for(int x = 2; x < in->cols - num; x++){
                if(in->data[ y * in->step + x * in->elemSize()] <= 0){
                    continue;
                }
                else{
                    int e = 0;
                    int e1 = 0;
                    int e2 = 0;
                    int e3 = 0;
                    int e4 = 0;
                    int s = 0;
                    for(int k = 0; k < num; k++){
                        e1 += (in->data[ (y - k) * in->step + x * in->elemSize()] - 1) * efilter[k];
                        e1 += (out->data[ (y + k) * out->step + x * out->elemSize()] -1)* efilter[k];
                        e3 += (in->data[ y  * in->step + (x - k) * in->elemSize()] - 1) * efilter[k];
                        e3 += (out->data[ y * out->step + (x + k) * out->elemSize()] -1)* efilter[k];

                        e2 += (in->data[ (y + k) * in->step + x * in->elemSize()] - 1) * efilter[k];
                        e2 += (out->data[ (y - k) * out->step + x * out->elemSize()] -1)* efilter[k];
                        e4 += (in->data[ y  * in->step + (x + k) * in->elemSize()] - 1) * efilter[k];
                        e4 += (out->data[ y * out->step + (x - k) * out->elemSize()] -1)* efilter[k];

                    }
                    e = max(max(e1, e2), max(e3, e4));
                    if( e > 0){
                        edge->data[ y * edge->step + x * edge->elemSize()] = 255;
                    }
                }
            }
    }
}

bool BoundaryDetector::calc_g(Mat *src, Vector2f *g){
    vector<vector<Point> > contours; 
    findContours(*src, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(), compareContourAreas);
    int calcnum = 2;

    if(contours.size() > 0){
        int i = contours.size() - 1;
        int count=contours.at(i).size();
        double x=0; double y=0;
        for(int j=0;j<count;j++){
            x+=contours.at(i).at(j).x;
            y+=contours.at(i).at(j).y;
        } 
        x/=count;
        y/=count;
        g->x() += x;
        g->y() += y;
    return true;
    }
    else{
        return false;
    }
}


void BoundaryDetector::findLine(Mat *src, Vector2f g, Vector2f *norm_start, Vector2f *norm){
    vector<Vec4i> lines;
    HoughLinesP(*src, lines, 1, CV_PI/180, 80, 30, 10 );
    Vector2f start(0, 0);
    Vector2f end(0, 0);
    Vector2f vec(0, 0);
    Vector2f norm_;
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vector2f start_, end_, vec_;
        start_.x() = lines[i][0];
        start_.y() = lines[i][1];
        end_.x() = lines[i][2];
        end_.y() = lines[i][3];
        vec_ = end_ - start_;
        vec_ /= vec_.norm();
        start += start_;
        end += end_;
        vec += vec_;
    }
    start /= lines.size();
    end /= lines.size();
    vec /= lines.size();
    *norm_start = 0.5 * (start + end);
    Vector2f vecg = g - start;
    norm_ << vec.y(), -vec.x();
    norm_ = norm_.dot(vecg) * norm_;
    norm_ /= norm_.norm();
    *norm = norm_;
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

    Mat in1_image = Mat::zeros(Size(org->cols, org->rows), CV_8U);
    Mat out1_image = Mat::zeros(Size(org->cols, org->rows), CV_8U);
    Mat edge = Mat::zeros(Size(org->cols, org->rows), CV_8U);
    
    extract_rgb(org, &in1_image, blue_r, blue_g, blue_b, blue_th);
    extract_rgb(org, &out1_image, green_r, green_g, green_b, green_th);
    detect_edge(&in1_image, &out1_image, &edge);
    Vector2f g;
    if(calc_g(&out1_image, &g)){
        findLine(&edge, g, norm_start, norm);
        return true;
    }
    else{
        return false;
    }
    return true;
}

