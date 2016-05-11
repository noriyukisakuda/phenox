#要求するcmakeのバージョン
cmake_minimum_required(VERSION 2.8)
#プロジェクト名
PROJECT(SocketIOTest)

#ヘッダファイルの場所を指定
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
#ライブラリの場所を指定
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/Release)

##コンパイラオプション
set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -fPIC -static-libstdc++")
#デバッグモードのオプション
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -ggdb")
#リリースモードのオプション
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#実行ファイル生成方法
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src ALL_SRC)
add_executable(SocketIOTest ${ALL_SRC})

#ライブラリのリンク
target_link_libraries(SocketIOTest sioclient)
#pthreadの追加
find_package(Threads REQUIRED)
target_link_libraries(SocketIOTest "${CMAKE_THREAD_LIBS_INIT}")

install(TARGETS SocketIOTest
    DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

