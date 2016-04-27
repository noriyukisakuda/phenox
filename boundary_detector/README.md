# phenox sample code
This is a test folder of boundary detector

##Install
This class includes Eigen.
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
Then, just use get_norm(Mat &src, Vector2f &norm_start, Vector2f &norm)
```c++
VideoCapture cap(0);
Mat src;
cap >> src;
Vector2f norm_start, norm;
bd.get_norm(&src, &norm_start, &norm);
