cmake_minimum_required(VERSION 3.28)
project(ParallelProg)

set(CMAKE_CXX_STANDARD 23)

add_executable(ParallelProg TaskBag.cpp
        tbag.cpp
        tbag.h)

set_target_properties(ParallelProg PROPERTIES LINK_FLAGS "-Wl,--stack=4294967295")