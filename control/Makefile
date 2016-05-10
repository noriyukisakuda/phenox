SRCS_C = $(shell find . -name "*.c")
SRCS_CPP = $(shell find . -name "*.cpp" )

CC = gcc
CXX = g++

CFLAGS = -g -I../../library/headers/ -Iinclude -lm `pkg-config --cflags opencv`
CXXFLAGS = -Wall -O2 -std=c++0x

LIBRARY_PATH = ../../library/px_library.a `pkg-config --libs opencv` -pthread -lturbojpeg libsioclient.a

OBJS = $(SRCS_CPP:.cpp=.o) $(SRCS_C:.c=.o) 

TARGET = main

.PHONY: all clean

all:$(OBJS)
	$(CXX) -o $(TARGET) $^ $(LIBRARY_PATH) $(CFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@


%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

main.o:main.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

memcheck:
	valgrind --tool=memcheck --leak-check=full ./$(TARGET)

contextcheck:
	valgrind -v ./$(TARGET)
