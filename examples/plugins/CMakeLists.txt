add_library(
  clap-plugin SHARED
  clap-entry.cc
  parameters.cc
  parameters.hh
  plugin.cc
  plugin.hh
  plugin-helper.cc
  plugin-helper.hh
  gain/gain.hh
  gain/gain.cc)

set_target_properties(clap-plugin PROPERTIES CXX_STANDARD 20)
