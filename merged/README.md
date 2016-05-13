## Phenoxの反射制御とりあえず動いた部分

 * 反射の際すべるので改善が必要
 * 制御ループに直接書いてしまっているのでクラスをつくって整理
 * 今のところ一つの線に対して反射（わかりにくいので）

## ARマーカーの認識部分
Use Library
*Eigen(割愛）
*Aruco

### Aruco
公式より本体のtarファイルと、テストデータをdownload([公式](http://www.uco.es/investiga/grupos/ava/node/26 "Aruco公式")、([ソース置き場](https://sourceforge.net/projects/aruco/files/ "ソース置き場")
今回の開発では1.2.5を用いた。
始めに依存関係をインストールする
`sudo apt-get update && sudo apt-get upgrade`
`sudo apt-get install　libicu-dev freeglut3 freeglut3-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libxine-dev`

phenox内でこれをビルドする際、ホストPCと異なりsrcファイルの一部を削除しないとビルドが出来ないのでこれを削除してからビルドする。（chromaticmaskは1.2.5からの新機能で今回は活用しなくても師匠はないものである。）

makeの際に-lopencv_gpuがないと怒られるのでリンクをはる。  
`ln -s /usr/lib/libopencv_gpu.so.2.3 /usr/lib/libopencv_gpu.so`

その後ビルドする。  
`cd aruco-1.2.5`
`rm -rf src/chromaticmask.cpp src/chromaticmask.h`
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

実行の際にlibaruco.so.1.2がないと怒られる場合がある。その場合、pathをLD_LIBRARY_PATHに追加する必要がある。  
`export LD_LIBRARY_PATH=/your/path/to/aruco.so.1.2:$LD_LIBRARY_PATH`


