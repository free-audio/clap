cmake_minimum_required(VERSION 3.20)
project(CLAP C CXX)

set(ENABLE_CLAP_GLUE TRUE CACHE BOOL "Enables the helper glue code")
set(ENABLE_CLAP_HOST FALSE CACHE BOOL "Enables the example host")
set(ENABLE_CLAP_PLUGINS FALSE CACHE BOOL "Enables the example plugins")
set(ENABLE_CLAP_GUI FALSE CACHE BOOL "Enables the plugin GUI framework")

include_directories(include)

add_executable(clap-compile-test-c EXCLUDE_FROM_ALL src/main.c)
add_executable(clap-compile-test-cpp EXCLUDE_FROM_ALL src/main.cc)
add_custom_target(clap-compile-tests DEPENDS clap-compile-test-c clap-compile-test-cpp)

add_subdirectory(examples)

install(DIRECTORY include DESTINATION "${CMAKE_INSTALL_PREFIX}")