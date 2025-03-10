cmake_minimum_required(VERSION 3.28)
project(ParallelProg)

set(CMAKE_CXX_STANDARD 23)

add_executable(ParallelProg TaskBag.cpp
        tbag.cpp
        tbag.h)
