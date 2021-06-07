add_library(
  clap-plugins SHARED
  clap-entry.cc
  parameters.cc
  parameters.hh
  plugin-helper.cc
  plugin-helper.hh
  gain/gain.hh
  gain/gain.cc)
target_link_libraries(clap-plugins clap-plugin-glue)
set_target_properties(clap-plugins PROPERTIES CXX_STANDARD 20)
