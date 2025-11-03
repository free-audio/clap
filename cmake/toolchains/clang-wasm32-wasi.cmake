# Simple toolchain example that targets wasm32 using wasi
# On archlinux:
#   pacman -S clang wasi-compiler-rt wasi-libc wasi-libc++ wasi-libc++abi
# On macOS:
#   homebrew install wasi-libc wasi-runtimes llvm
#
# For other OSes, make a copy of this file and do the necessary adjustments.

set(CMAKE_SYSTEM_NAME "WASI")
set(CMAKE_CROSSCOMPILING TRUE)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
    set(CMAKE_SYSROOT "/usr/share/wasi-sysroot")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
    set(CMAKE_SYSROOT "/opt/homebrew/share/wasi-sysroot")
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
    set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
    set(CMAKE_RANLIB "/opt/homebrew/opt/llvm/bin/llvm-ranlib")
endif()

# -nostdlib
set(CMAKE_C_FLAGS_INIT "--target=wasm32-wasip1-threads")
set(CMAKE_CXX_FLAGS_INIT "--target=wasm32-wasip1-threads -fwasm-exceptions")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--export-all -Wl,--no-entry -lpthread -Wl,--shared-memory -Wl,--import-memory")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-Wl,--export-all -Wl,--no-entry -lpthread -Wl,--shared-memory -Wl,--import-memory")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-Wl,--export-all -Wl,--no-entry -lpthread -Wl,--shared-memory -Wl,--import-memory")

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
