cmake_minimum_required(VERSION 2.6)
project(CLAP C CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)
add_subdirectory(examples)
