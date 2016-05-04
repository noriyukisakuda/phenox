# phenox sample code
This is a sample of phenox main.cpp
Separated into two thread.

Please write control code in timer handler.

Please write OpenCV code in main.

##Install
This class uses Eigen.
So you have to copy Eigen folder to `/usr/local/include`

Eigen is available at [Eigen download](http://eigen.tuxfamily.org/index.php?title=Main_Page#Download)

##Usage
First, include boundary_detector.h
```c++
#include "boundary_detector.h"
```
and, make object of BoundaryDetector.
```c++
BoundaryDetector bd;
```
Then, just use get_norm(Mat *src, Vector2f *norm_start, Vector2f *norm, Vector2f *norm_start2, Vector2f *norm2)
```c++
VideoCapture cap(0);
Mat src;
cap >> src;
Vector2f norm_start, norm, norm_start2, norm2;
int boundary_num = bd.get_norm(&org, &norm_start, &norm, &norm_start2, &norm2)
