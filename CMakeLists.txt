cmake_minimum_required(VERSION 3.25)

project(RGBtoYUVConverter)

set(SRC main.cpp src/bmp.cpp src/yuv.cpp)
include_directories("include/")

add_executable(converter ${SRC})

file(COPY samples DESTINATION ${CMAKE_BINARY_DIR})
