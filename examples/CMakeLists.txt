if (ENABLE_CLAP_HOST)
    add_subdirectory(host)
endif()

if (ENABLE_CLAP_SYNAESTHESIS OR ENABLE_CLAP_PLUGINS)
    add_subdirectory(synaesthesis)
endif()

if (ENABLE_CLAP_PLUGINS)
    add_subdirectory(plugins)
endif()