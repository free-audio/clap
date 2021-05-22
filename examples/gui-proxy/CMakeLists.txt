add_library(clap-gui-proxy STATIC
    proxy.cc
    proxy.hh
)
set_target_properties(clap-gui-proxy PROPERTIES POSITION_INDEPENDENT_CODE TRUE)