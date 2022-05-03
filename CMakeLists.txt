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

add_library(clap-plugin-template MODULE EXCLUDE_FROM_ALL src/plugin-template.c)
target_link_libraries(clap-plugin-template clap-core)
set_target_properties(clap-plugin-template PROPERTIES C_STANDARD 11)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(clap-compile-c PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(clap-compile-cpp PRIVATE -Wall -Wextra -pedantic)
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(clap-compile-c PRIVATE -Werror=pragma-pack)
    target_compile_options(clap-compile-cpp PRIVATE -Werror=pragma-pack)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(clap-plugin-template PRIVATE -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/src/linux-my_plug.version)
    target_link_libraries(clap-plugin-template PRIVATE -Wl,-z,defs)
    set_target_properties(clap-plugin-template PROPERTIES SUFFIX ".clap" PREFIX "")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_options(clap-plugin-template PRIVATE -exported_symbols_list ${CMAKE_CURRENT_SOURCE_DIR}/src/macos-symbols.txt)

    set_target_properties(clap-plugin-template PROPERTIES
                BUNDLE True
                BUNDLE_EXTENSION clap
                MACOSX_BUNDLE_GUI_IDENTIFIER com.my_company.my_plug
                MACOSX_BUNDLE_BUNDLE_NAME my_plug
                MACOSX_BUNDLE_BUNDLE_VERSION "1"
                MACOSX_BUNDLE_SHORT_VERSION_STRING "1"
                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins.plist.in
                )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(clap-plugin-template PROPERTIES SUFFIX ".clap" PREFIX "")
endif()

add_test(NAME test-clap-compile-c COMMAND clap-compile-c)
add_test(NAME test-clap-compile-cpp COMMAND clap-compile-cpp)

add_custom_target(clap-tests)
add_dependencies(clap-tests clap-compile-c clap-compile-cpp)
