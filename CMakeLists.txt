cmake_minimum_required(VERSION 3.17)
project(CLAP C CXX)

# If you use clap as a submodule of your plugin you need some interface projects
# to allow you to link. clap-core gives you the include directory and clap-plugin-core
# gives you the core + plugin-glue.
add_library(clap-core INTERFACE)
target_include_directories(clap-core INTERFACE include)

install(DIRECTORY include DESTINATION "${CMAKE_INSTALL_PREFIX}" OPTIONAL EXCLUDE_FROM_ALL)

add_executable(clap-compile-test-c EXCLUDE_FROM_ALL src/main.c)
target_link_libraries(clap-compile-test-c clap-core)
set_target_properties(clap-compile-test-c PROPERTIES C_STANDARD 11)

add_executable(clap-compile-test-cpp EXCLUDE_FROM_ALL src/main.cc)
target_link_libraries(clap-compile-test-cpp clap-core)
set_target_properties(clap-compile-test-cpp PROPERTIES CXX_STANDARD 14)

add_custom_target(clap-compile-tests DEPENDS clap-compile-test-c clap-compile-test-cpp)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(clap-compile-test-c PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(clap-compile-test-cpp PRIVATE -Wall -Wextra -pedantic)
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(clap-compile-test-c PRIVATE -Werror=pragma-pack)
    target_compile_options(clap-compile-test-cpp PRIVATE -Werror=pragma-pack)
endif()
