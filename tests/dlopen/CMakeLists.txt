add_library(global1 SHARED lib1.c)
add_library(global2 SHARED lib2.c)
target_link_libraries(global1 global2)

add_executable(dlopen dlopen.c)
target_link_libraries(dlopen dl)
