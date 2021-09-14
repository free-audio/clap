add_library(clap-io STATIC
    remote-channel.hh
    remote-channel.cc
    buffer.hh
)

set_target_properties(clap-io PROPERTIES CXX_STANDARD 17)
set_target_properties(clap-io PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_include_directories(clap-io INTERFACE .)