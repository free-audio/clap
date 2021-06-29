find_package(Boost REQUIRED COMPONENTS serialization iostreams)

add_library(
  clap-plugins SHARED
  clap-entry.cc
  parameters.cc
  parameters.hh
  parameter-interpolator.hh
  plugin-helper.cc
  plugin-helper.hh
  stream-helper.hh

  abstract-gui.hh
  remote-gui.hh
  remote-gui.cc
  remote-channel.hh
  remote-channel.cc

  dc-offset/dc-offset.hh
  dc-offset/dc-offset.cc
  gain/gain.hh
  gain/gain.cc)
target_link_libraries(clap-plugins clap-plugin-glue Boost::serialization Boost::iostreams)
set_target_properties(clap-plugins PROPERTIES CXX_STANDARD 20)

install(TARGETS clap-plugins DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clap")