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

  dc-offset/dc-offset.hh
  dc-offset/dc-offset.cc
  gain/gain.hh
  gain/gain.cc)
target_link_libraries(clap-plugins clap-plugin-glue clap-io Boost::serialization Boost::iostreams)
target_link_libraries(clap-plugins -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/linux-clap-plugins.version)
set_target_properties(clap-plugins PROPERTIES CXX_STANDARD 20 SUFFIX ".clap" PREFIX "")

install(TARGETS clap-plugins DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clap")