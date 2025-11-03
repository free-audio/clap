# Simple toolchain example that targets wasm32 using emscripten
# On archlinux:
#   pacman -S clang emscripten
# On macOS:
#   brew install emscripten
#
# For other OSes, make a copy of this file and do the necessary adjustments.

set(CMAKE_SYSTEM_NAME "Emscripten")
set(CMAKE_CROSSCOMPILING TRUE)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
    set(CMAKE_C_COMPILER "/usr/lib/emscripten/emcc")
    set(CMAKE_CXX_COMPILER "/usr/lib/emscripten/em++")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
    set(CMAKE_C_COMPILER "/opt/homebrew/bin/emcc")
    set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/em++")
else()
    set(CMAKE_C_COMPILER "emcc")
    set(CMAKE_CXX_COMPILER "em++")
endif()

set(CMAKE_C_FLAGS_INIT "-pthread")
set(CMAKE_CXX_FLAGS_INIT "-pthread -fwasm-exceptions")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-pthread")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-pthread")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-pthread")
