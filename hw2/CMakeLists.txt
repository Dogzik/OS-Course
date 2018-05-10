cmake_minimum_required(VERSION 3.10)
project(hw2)

set(CMAKE_CXX_STANDARD 14)

add_executable(grep main.cpp stream_finder.cpp stream_finder.h path.cpp path.h autocloseable.cpp autocloseable.h)