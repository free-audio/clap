set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Qml REQUIRED)

add_executable(clap-gui
    main.cc

    application.hh
    application.cc
)
target_link_libraries(clap-gui Qt6::Qml Qt6::Widgets Qt6::Core)

set_target_properties(clap-gui PROPERTIES CXX_STANDARD 17)
install(TARGETS clap-gui DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")