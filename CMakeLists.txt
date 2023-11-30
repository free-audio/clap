cmake_minimum_required(VERSION 3.17)
enable_testing()

# Extract the version from header file
file(READ "include/clap/version.h" clap_version_header)
string(REGEX MATCH "CLAP_VERSION_MAJOR ([0-9]+)" _ ${clap_version_header})
set(CLAP_VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "CLAP_VERSION_MINOR ([0-9]+)" _ ${clap_version_header})
set(CLAP_VERSION_MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "CLAP_VERSION_REVISION ([0-9]+)" _ ${clap_version_header})
set(CLAP_VERSION_REVISION ${CMAKE_MATCH_1})

message(STATUS "CLAP version: ${CLAP_VERSION_MAJOR}.${CLAP_VERSION_MINOR}.${CLAP_VERSION_REVISION}")

project(CLAP LANGUAGES C CXX VERSION ${CLAP_VERSION_MAJOR}.${CLAP_VERSION_MINOR}.${CLAP_VERSION_REVISION})

option(CLAP_BUILD_TESTS "Should CLAP build tests and the like?" OFF)

# If you use clap as a submodule of your plugin you need some interface projects
# to allow you to link
add_library(clap INTERFACE)
target_include_directories(clap INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# `clap-core` is deprecated, please `clap` instead.
add_library(clap-core ALIAS clap)

include(GNUInstallDirs)
install(DIRECTORY "include/clap" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(TARGETS clap EXPORT clap INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(EXPORT clap DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/clap" FILE "clap-config.cmake")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/clap-config-version.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/clap-config-version.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/clap")

# In addition to the above generated `clap-config.cmake` file, we'll also
# provide a pkg-config file to make it easier to consume this library in a
# portable way
configure_file(clap.pc.in "${CMAKE_CURRENT_BINARY_DIR}/clap.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/clap.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

# clap-tests should always be available, to avoid build failing here and there
# because the target doesn't exists
add_custom_target(clap-tests)

if (${CLAP_BUILD_TESTS})
    message(STATUS "Including CLAP tests, compile tests, and versions")
    include(CheckIncludeFile)

    macro(clap_compile_cpp SUFFIX EXT STDC STDCPP)
        add_executable(clap-compile-${SUFFIX} EXCLUDE_FROM_ALL src/main.${EXT})
        target_link_libraries(clap-compile-${SUFFIX} clap)
        set_target_properties(clap-compile-${SUFFIX} PROPERTIES
            C_STANDARD ${STDC}
            CXX_STANDARD ${STDCPP})
        add_test(NAME test-clap-compile-${SUFFIX} COMMAND clap-compile-${SUFFIX})
        add_dependencies(clap-tests clap-compile-${SUFFIX})

        if (${EXT} STREQUAL "cc")
            target_compile_definitions(clap-compile-${SUFFIX} PRIVATE CLAP_COMPILE_TEST_CXX_VERSION=${STDCPP})
        endif()

        if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
            target_compile_options(clap-compile-${SUFFIX} PRIVATE -Wall -Wextra -pedantic)
        endif()

        if (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
            target_compile_options(clap-compile-${SUFFIX} PRIVATE -Werror=pragma-pack)
        endif()
    endmacro()

    clap_compile_cpp(c11    c 11 11)
    clap_compile_cpp(cpp11 cc 11 11)
    clap_compile_cpp(cpp14 cc 11 14)
    if(${CMAKE_VERSION} VERSION_LESS "3.21")
       message(STATUS "Skipping C17 tests due to older CMAKE_VERSION ${CMAKE_VERSION}")
    else()
      clap_compile_cpp(c17    c 17 17)
    endif()
    clap_compile_cpp(cpp17 cc 17 17)
    clap_compile_cpp(cpp20 cc 17 20)

    check_include_file(threads.h CLAP_HAS_THREADS_H)

    add_library(clap-plugin-template MODULE EXCLUDE_FROM_ALL src/plugin-template.c)
    target_link_libraries(clap-plugin-template PRIVATE clap)
    set_target_properties(clap-plugin-template PROPERTIES C_STANDARD 11)
    add_dependencies(clap-tests clap-plugin-template)

    if(CLAP_HAS_THREADS_H)
        target_compile_definitions(clap-plugin-template PRIVATE CLAP_HAS_THREADS_H)
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
endif()
