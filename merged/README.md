# Phenoxの反射制御とりあえず動いた部分

 * 反射の際すべるので改善が必要
 * 制御ループに直接書いてしまっているのでクラスをつくって整理
 * 今のところ一つの線に対して反射（わかりにくいので）

#ライン検知部分
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

### opencv sample
```c++
VideoCapture cap(0);
Mat src;
cap >> src;
Vector2f norm_start, norm, norm_start2, norm2;
int boundary_num = bd.get_norm(&org, &norm_start, &norm, &norm_start2, &norm2)
```

### phenox sample
```c++
Mat mat;
IplImage *testImage;  
Vector2f norm_start, norm, norm_start2, norm2;
if(pxget_imgfullwcheck(cameraid,&testImage) == 1) {	
  mat = cvarrToMat(testImage);
  int gn = bd.get_norm(&mat, &norm_start, &norm, &norm_start2, &norm2);
}
```


get_norm returns the number of detected edge.

If it returns 1, it means that there is only one edge.
The norm is substituted in norm and, the start point of the norm is substituted in norm_start.

If it returns 2, there are two edges.
Then the two norms and start points are substituted in norm, norm2, norm_start, norm_start2.


# ARマーカーの認識部分
Use Library
*Eigen(割愛）
*Aruco

## Aruco
公式より本体のtarファイルと、テストデータをdownload([公式](http://www.uco.es/investiga/grupos/ava/node/26 "Aruco公式")、([ソース置き場](https://sourceforge.net/projects/aruco/files/ "ソース置き場")
今回の開発では1.2.5を用いた。
始めに依存関係をインストールする
`sudo apt-get update && sudo apt-get upgrade`
`sudo apt-get install　libicu-dev freeglut3 freeglut3-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libxine-dev`

phenox内でこれをビルドする際、ホストPCと異なりsrcファイルの一部を削除しないとビルドが出来ないのでこれを削除してからビルドする。（chromaticmaskは1.2.5からの新機能で今回は活用しなくても師匠はないものである。）

`cd aruco-1.2.5`
`rm -rf src/chromaticmask.cpp chromaticmask.h`
`mkdir build`
`cd build`
`cmake ..`
`make`


自分のWorkSpaceにinclude,libフォルダを作成し、　Aruco内のインクルードファイルと
libファイルをコピー、libaruco.so.1.2.5に関してはgitからpullしたものでは活用できず
`not recognized`といったErrorが出るので**必ずphenox内でbuildしたものからコピーする**こと

`cd YOUR_WORK_DIR`
`$ mkdir include lib`
`$ cp ARUCO_INSTALL_DIR/aruco-1.2.5/src/*.h include/.`
`$ cp ARUCO_INSTALL_DIR/build/src/libaruco.so.1.2.5 libaruco.so.1.2 libaruco.so lib/.`



またtestデータ内の(カメラのパラメーター)yamlファイルもコピー（本来は自作したほうがいい）
`$cp ARUCO_TEST_DATA/board/intrinsics.yml`


