set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)

add_executable(clap-gui
    main.cc
)

set_target_properties(clap-gui PROPERTIES CXX_STANDARD 17)