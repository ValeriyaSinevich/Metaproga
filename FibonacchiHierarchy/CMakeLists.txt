cmake_minimum_required(VERSION 3.6)
project(typelists)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/home/valeriyasin/Programms/loki-0.1.7/include/loki)

set(SOURCE_FILES FibonacchiHierarchy.cpp)
add_executable(typelists ${SOURCE_FILES})