add_executable(clap-alsa-host clap-alsa-host.c)
target_link_libraries(clap-alsa-host asound dl m)
