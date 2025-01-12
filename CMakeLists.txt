cmake_minimum_required(VERSION 3.25)

project(RGBtoYUVConverter)

set(SRC main.cpp src/bmp.cpp)
include_directories("include/")

add_executable(converter ${SRC})

file(COPY data DESTINATION ${CMAKE_BINARY_DIR})
