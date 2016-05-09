Use Library
Eigen(割愛）
Aruco

Aruco
公式より本体のtarファイルと、テストデータをdownload(公式http://www.uco.es/investiga/grupos/ava/node/26、ソース置き場 https://sourceforge.net/projects/aruco/files/)
今回の開発では1.2.5を用いた。
phenox内でこれをビルドする際、ホストPCと異なりsrcファイルの一部を削除しないとビルドが出来ないのでこれを削除してからビルドする

'''
cd aruco-1.2.5
rm -rf src/chromaticmask.cpp chromaticmask.h
mkdir build
cd build
cmake ..
make
'''




自分のWorkSpaceにinclude,libフォルダを作成し、　Aruco内のインクルードファイルと
libファイルをコピー

'''
$ cd YOUR_WORK_DIR
$ mkdir include lib
$ cp ARUCO_INSTALL_DIR/aruco-1.2.5/src/*.h include/.
$ cp ARUCO_INSTALL_DIR/build/src/libaruco.so.1.2.5 libaruco.so.1.2 libaruco.so lib/.
'''

またtestデータ内の(カメラのパラメーター)yamlファイルもコピー（本来は自作したほうがいい）
$cp ARUCO_TEST_DATA/board/intrinsics.yml


