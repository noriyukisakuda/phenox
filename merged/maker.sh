export CC=arm-linux-gnueabihf-gcc
export CXX=arm-linux-gnueabihf-g++
export AR=arm-linux-gnueabihf-ar
mkdir build
cd build
cmake ../
cmake --build . --config Release --target install
