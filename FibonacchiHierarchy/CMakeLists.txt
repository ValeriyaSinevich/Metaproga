cmake_minimum_required(VERSION 3.6)
project(typelists)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES FibonacchiHierarchy.cpp TypeManip.h)
add_executable(typelists ${SOURCE_FILES})
