add_library(clap-plugin-glue EXCLUDE_FROM_ALL STATIC clap-plugin.cc
                                              clap-plugin.hh)
set_target_properties(clap-plugin-glue PROPERTIES CXX_STANDARD 17
                                                  POSITION_INDEPENDENT_CODE ON)
target_include_directories(clap-plugin-glue INTERFACE .)

install(FILES clap-plugin.hh DESTINATION "${CMAKE_INSTALL_PREFIX}/include/clap/glue")
install(TARGETS clap-plugin-glue DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")