cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0100 NEW)  # handle .hh files
project(CLAP C CXX)

# If you use clap as a submodule of your plugin you need some interface projects
# to allow you to link. clap-core gives you the include directory and clap-plugin-core
# gives you the core + plugin-glue.
add_library(clap-core INTERFACE)
target_include_directories(clap-core INTERFACE include)

install(DIRECTORY include DESTINATION "${CMAKE_INSTALL_PREFIX}")

add_executable(clap-compile-test-c EXCLUDE_FROM_ALL src/main.c)
target_link_libraries(clap-compile-test-c clap-core)
add_executable(clap-compile-test-cpp EXCLUDE_FROM_ALL src/main.cc)
target_link_libraries(clap-compile-test-cpp clap-core)
add_custom_target(clap-compile-tests DEPENDS clap-compile-test-c clap-compile-test-cpp)
