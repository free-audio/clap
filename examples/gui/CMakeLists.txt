set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Quick REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(clap-gui
    main.cc

    application.hh
    application.cc

    parameter-proxy.hh
    parameter-proxy.cc
    plugin-proxy.hh
    plugin-proxy.cc
)
target_link_libraries(clap-gui clap-io Qt6::Widgets Qt6::Quick)

set_target_properties(clap-gui PROPERTIES CXX_STANDARD 17)
install(TARGETS clap-gui DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")