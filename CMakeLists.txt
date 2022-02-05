cmake_minimum_required(VERSION 3.17)
enable_testing()
project(CLAP C CXX)

# If you use clap as a submodule of your plugin you need some interface projects
# to allow you to link. clap-core gives you the include directory and clap-plugin-core
# gives you the core + plugin-glue.
add_library(clap-core INTERFACE)
target_include_directories(clap-core INTERFACE include)

install(DIRECTORY include DESTINATION "." OPTIONAL EXCLUDE_FROM_ALL)

add_executable(clap-compile-c EXCLUDE_FROM_ALL src/main.c)
target_link_libraries(clap-compile-c clap-core)
set_target_properties(clap-compile-c PROPERTIES C_STANDARD 11)

add_executable(clap-compile-cpp EXCLUDE_FROM_ALL src/main.cc)
target_link_libraries(clap-compile-cpp clap-core)
set_target_properties(clap-compile-cpp PROPERTIES CXX_STANDARD 14)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(clap-compile-c PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(clap-compile-cpp PRIVATE -Wall -Wextra -pedantic)
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(clap-compile-c PRIVATE -Werror=pragma-pack)
    target_compile_options(clap-compile-cpp PRIVATE -Werror=pragma-pack)
endif()

add_test(NAME test-clap-compile-c COMMAND clap-compile-c)
add_test(NAME test-clap-compile-cpp COMMAND clap-compile-cpp)

add_custom_target(clap-tests)
add_dependencies(clap-tests clap-compile-c clap-compile-cpp)
