#include <opencv2/opencv.hpp>
#include <iostream>
#include <Eigen/Core>
#include "boundary_detector.h"
#include <time.h>
#include <sys/time.h>

using namespace std;
using namespace cv;
using namespace Eigen;

BoundaryDetector::BoundaryDetector(){
    blue_r = 0.77;
    blue_g = -0.12;
    blue_b = -0.63;
    blue_th = 30;
    
    yellow_r = -0.46;
    yellow_g =  0.81;
    yellow_b = -0.34;
    yellow_th = 10;

    // blue_r = -0.82;
    // blue_g = 0.49;
    // blue_b = 0.29;
    // blue_th = 20;
    //
    // yellow_r = -0.41;
    // yellow_g =  0.84;
    // yellow_b = -0.36;
    // yellow_th = 22;
    //
    for(int i = 0;i < 30; i++)
        efilter[i] = 1;

    edge_num_ = 10;
    skip_step_ = 2;
}

void BoundaryDetector::extract_rgb(Mat* src, Mat* dst, double cr, double cg, double cb, double th){

    for(int y = 0; y < src->rows; y=y+skip_step_){
            for(int x = 0; x < src->cols; x=x+skip_step_){
                    // 画像のチャネル数分だけループ。白黒の場合は1回、カラーの場合は3回　　　　　
                    double r = src->data[ y * src->step + x * src->elemSize()];
                    double g = src->data[ y * src->step + x * src->elemSize() + 1];
                    double b = src->data[ y * src->step + x * src->elemSize() + 2];
                    double gray = r * cr + g * cg + b * cb;
                    if(gray > th){
                        dst->data[ y / skip_step_ * dst->step + x / skip_step_ * dst->elemSize()] = 2;
                    }
                    else{
                        dst->data[ y / skip_step_ * dst->step + x / skip_step_ * dst->elemSize()] = 0;
                    }
            }
    }
    return;
}

void BoundaryDetector::detect_edge(Mat* in, Mat* out, Mat* edge){
    for(int y = 2; y < in->rows - edge_num_; y++){
            for(int x = 2; x < in->cols - edge_num_; x++){
                if(in->data[ y * in->step + x * in->elemSize()] <= 0){
                    continue;
                }
                else{
                    int e = 0;
                    int e1 = 0;
                    int e2 = 0;
                    int e3 = 0;
                    int e4 = 0;
                    for(int k = 0; k < edge_num_; k++){
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

bool BoundaryDetector::calc_g(Mat *in, Vector2f *g, Vector2f center, int range){
    g->x() = 0;
    g->y() = 0;
    int cnt = 0;
    int xmin = std::max((int)center.x() - range, 0);
    int ymin = std::max((int)center.y() - range, 0);
    int xmax = std::min((int)center.x() + range, in->cols);
    int ymax = std::min((int)center.y() + range, in->rows);
    // cout << "xmin xmax = " << xmin << ", " << xmax << endl;
    // cout << "ymin ymax = " << ymin << ", " << ymax << endl;
    for(int y = ymin; y < ymax; y=y+4){
        for(int x = xmin; x < xmax; x=x+4){
            if(in->data[ y * in->step + x * in->elemSize()] <= 0){
                continue;
            }
            else{
                g->x() += x;
                g->y() += y;
                cnt++;
            }
        }
    }
    g->x() /= cnt;
    g->y() /= cnt;
    return true;
}

int BoundaryDetector::count_direction(Mat *out, Vector2f center, Vector2f norm, int range){
    int cnt1 = 0;
    int cnt2 = 0;
    int xmin = std::max((int)center.x() - range, 0);
    int ymin = std::max((int)center.y() - range, 0);
    int xmax = std::min((int)center.x() + range, out->cols);
    int ymax = std::min((int)center.y() + range, out->rows);
    for(int y = ymin; y < ymax; y++){
        for(int x = xmin; x < xmax; x++){
            // cout << " " << (int)out->data[ y * out->step + x * out->elemSize()];
            if(out->data[ y * out->step + x * out->elemSize()] <= 0){
                // cout << "" << (int)out->data[ y * out->step + x * out->elemSize()];
                continue;
            }
            else{
                Vector2f vecx(x, y);
                vecx = vecx - center;
                // cout << " x, y = " << x << ", " << y << endl;
                // cout  << "vecx * norm = " << vecx.dot(norm) << endl;
                // cout  << "d = " << (vecx - vecx.dot(norm) * norm).norm() << endl;
                // cout << "cnt1, cnt2 = " << cnt1 << ", " << cnt2 << endl;
                if((vecx - vecx.dot(norm) * norm).norm() < 40){
                // if(1){
                    if(vecx.dot(norm) > 0){
                    // cout << "cnt1 " << endl;
                        // cout << "+";
                        cnt1++;
                    }
                    else{
                        // cout << "cnt2 " << endl;
                        // cout << "-";
                        cnt2++;
                    }
                }
                else{
                    // cout << "1";
                }

            }
        }
        // cout << endl;
    }
    // cout << "cnt1, cnt2 = " << cnt1 << ", " << cnt2 << endl;
    if(cnt1 < cnt2)
        return 1;
    else
        return -1;
}


int BoundaryDetector::findLine(Mat *src, Mat *out1_image, Vector2f *norm_start1, Vector2f *norm1, Vector2f *norm_start2, Vector2f *norm2){
    vector<Vec4i> lines;
    HoughLinesP(*src, lines, 1, CV_PI/180, 30, 10, 10 );
    Vector2f start(0, 0);
    Vector2f end(0, 0);
    Vector2f vec(0, 0);
    Vector2f norm_, vecg;
    Vector2f start1, start2, end1, end2, vec1, vec2;
    if(lines.size() > 2){
        Mat mlines = Mat::zeros(Size(2, lines.size()), CV_32F);
        Vector2f start(0, 0);
        for(unsigned int i = 0; i < lines.size(); i++){
            Vector2f start_, end_, vec_;
            start_.x() = lines[i][0];
            start_.y() = lines[i][1];
            end_.x() = lines[i][2];
            end_.y() = lines[i][3];
            vec_ = end_ - start_;
            vec_ /= vec_.norm();
            mlines.at<float>(i, 3) = start_.x();
            mlines.at<float>(i, 4) = start_.y();
            mlines.at<float>(i, 0) = vec_.x();
            mlines.at<float>(i, 1) = vec_.y();
        }
        Mat labels = Mat::zeros(Size(1, lines.size()), CV_32F);
        Mat centers = Mat::zeros(Size(2, lines.size()), CV_32F);
        kmeans(mlines, 2, labels, cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0), 1, 0, centers); 
        Vector2f vec1(0, 0);
        Vector2f vec2(0, 0);
        Vector2f start1(0, 0);
        Vector2f start2(0, 0);
        Vector2f end1(0, 0);
        Vector2f end2(0, 0);
        vec1 << centers.at<float>(0, 0), centers.at<float>(0, 1);
        vec2 << centers.at<float>(1, 0), centers.at<float>(1, 1);
        double e = vec2.dot(vec1);
        int label0 = 0; int label1 = 0;
        for(int i = 0; i < labels.rows; i++){
            if(labels.at<int>(i) == 0){ 
                start1.x() += lines[i][0];
                start1.y() += lines[i][1];
                end1.x() += lines[i][2];
                end1.y() += lines[i][3];
                label0 += 1; 
            }
            else{
                start2.x() += lines[i][0];
                start2.y() += lines[i][1];
                end2.x() += lines[i][2];
                end2.y() += lines[i][3];
                label1 += 1;
            }
        }
        int max_label;
        if(label0 > label1){
            max_label = 0;
        }
        else{
            max_label = 1;
        }
        start1 /= label0 * 1.0;
        start2 /= label1 * 1.0;
        end1 /= label0 * 1.0;
        end2 /= label1 * 1.0;
        Vector2f g;
        if(fabs(e) > 0.8 || min(label0, label1) < 2){
            if(max_label == 0){
                *norm_start1 = 0.5 * (start1 + end1);
                norm_ << vec1.y(), -vec1.x();
            }
            else{
                *norm_start1 = 0.5 * (start2 + end2);
                norm_ << vec2.y(), -vec2.x();
            }
            norm_ /= norm_.norm();
            *norm1 = norm_;
            return 1;
        }
        else{
            *norm_start1 = 0.5 * (start1 + end1);
            norm_ << vec1.y(), -vec1.x();
            norm_ /= norm_.norm();
            *norm1 = norm_;

            *norm_start2 = 0.5 * (start2 + end2);
            norm_ << vec2.y(), -vec2.x();
            norm_ /= norm_.norm();
            *norm2 = norm_;
            return 2;
        }
    }
    return 0;
}

int BoundaryDetector::get_norm(Mat *org, Vector2f *norm_start1, Vector2f *norm1, Vector2f *norm_start2, Vector2f *norm2){
    Mat in1_image = Mat::zeros(Size(org->cols / skip_step_ + 1, org->rows / skip_step_ + 1), CV_8U);
    Mat out1_image = Mat::zeros(Size(org->cols / skip_step_ + 1, org->rows / skip_step_ + 1), CV_8U);
    Mat edge = Mat::zeros(Size(org->cols / skip_step_ + 1, org->rows / skip_step_ + 1), CV_8U);
    Vector2f g(0, 0);
    extract_rgb(org, &in1_image, yellow_r, yellow_g, yellow_b, yellow_th);
    extract_rgb(org, &out1_image, blue_r, blue_g, blue_b, blue_th);
    calc_g(&in1_image, &g, Vector2f(50, 50), 500);
    detect_edge(&out1_image, &in1_image, &edge);

    // /*
    // imshow("out", out1_image * 120);
    // imshow("in", in1_image * 120);
    // imshow("edge", edge);
    // out1_image = Mat::zeros(Size(org->cols / skip_step_ + 1, org->rows / skip_step_ + 1), CV_8U);

	/*
    static int image_count = 0;
    stringstream ss;
    ss << "px_image" << image_count << ".png";
    imwrite(ss.str(), edge);
    stringstream ssb;
    ssb << "px_image_blue" << image_count << ".png";
    imwrite(ssb.str(), out1_image * 120);
    stringstream ssy;
    ssy << "px_image_yellow" << image_count << ".png";
    imwrite(ssy.str(), in1_image * 120);
    cout << "save" << endl;
    image_count++;
*/

    int find = findLine(&edge, &out1_image, norm_start1, norm1, norm_start2, norm2);

    if(find == 1){
        *norm1 = *norm1 * count_direction(&out1_image, g, *norm1, 80);
    }
    if(find == 2){
        *norm1 = *norm1 * count_direction(&out1_image, g, *norm1, 80);
        *norm2 = *norm2 * count_direction(&out1_image, g, *norm2, 80);
    }
    *norm_start1 *= skip_step_;
    *norm_start2 *= skip_step_;


    return find;
}

