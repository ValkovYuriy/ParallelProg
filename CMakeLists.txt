cmake_minimum_required(VERSION 3.28)
project(ParallelProg)

set(CMAKE_CXX_STANDARD 23)

add_executable(OMP openMP.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set_target_properties(OMP PROPERTIES LINK_FLAGS "-Wl,--stack=4294967295")