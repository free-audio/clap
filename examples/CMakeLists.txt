add_subdirectory(common)

if(ENABLE_CLAP_GLUE)
  add_subdirectory(glue)
endif()

if(ENABLE_CLAP_HOST)
  add_subdirectory(host)
endif()

if(ENABLE_CLAP_GUI OR ENABLE_CLAP_PLUGINS)
  add_subdirectory(io)
  add_subdirectory(gui)
endif()

if(ENABLE_CLAP_PLUGINS)
  add_subdirectory(plugins)
endif()
