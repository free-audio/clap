cmake_minimum_required(VERSION 3.17)
enable_testing()
project(CLAP C CXX)

# If you use clap as a submodule of your plugin you need some interface projects
# to allow you to link. clap-core gives you the include directory and clap-plugin-core
# gives you the core + plugin-glue.
add_library(clap-core INTERFACE)
target_include_directories(clap-core INTERFACE include)

install(DIRECTORY include DESTINATION "." OPTIONAL EXCLUDE_FROM_ALL)

option(CLAP_BUILD_TESTS "Should CLAP build tests and the like?" OFF)

if (${CLAP_BUILD_TESTS})
    message(STATUS "Including clap tests, compile tests, and versions")
    add_custom_target(clap-tests)

    add_executable(clap-compile-c EXCLUDE_FROM_ALL src/main.c)

    macro(clap_compile_cpp SUFFIX EXT STDC STDCPP)
        add_executable(clap-compile-${SUFFIX} EXCLUDE_FROM_ALL src/main.${EXT})
        target_link_libraries(clap-compile-${SUFFIX} clap-core)
        set_target_properties(clap-compile-${SUFFIX} PROPERTIES
            C_STANDARD ${STDC}
            CXX_STANDARD ${STDCPP})
        add_test(NAME test-clap-compile-${SUFFIX} COMMAND clap-compile-${SUFFIX})
        add_dependencies(clap-tests clap-compile-${SUFFIX})

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
    clap_compile_cpp(c17    c 17 17)
    clap_compile_cpp(cpp17 cc 17 17)
    clap_compile_cpp(cpp20 cc 17 20)

    add_library(clap-plugin-template MODULE EXCLUDE_FROM_ALL src/plugin-template.c)
    target_link_libraries(clap-plugin-template PRIVATE clap-core)
    set_target_properties(clap-plugin-template PROPERTIES C_STANDARD 11)

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
        target_link_options(clap-plugin-template PRIVATE /EXPORT:clap_entry)
        set_target_properties(clap-plugin-template PROPERTIES SUFFIX ".clap" PREFIX "")
    endif()
endif()
