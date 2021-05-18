cmake_minimum_required(VERSION 3.20)
project(CLAP C CXX)

set(ENABLE_CLAP_HOST FALSE CACHE BOOL "Enables the example host")

include_directories(include)

add_executable(clap-compile-test-c EXCLUDE_FROM_ALL src/main.c)
add_executable(clap-compile-test-cpp EXCLUDE_FROM_ALL src/main.cc)

add_subdirectory(examples)