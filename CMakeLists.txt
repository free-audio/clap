cmake_minimum_required(VERSION 3.20)
project(CLAP C CXX)

include_directories(include)
add_executable(clap-compile-test-c src/main.c)
add_executable(clap-compile-test-cpp src/main.cc)