set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PortAudio REQUIRED IMPORTED_TARGET portaudio-2.0)

# PortMidi do not provide pkgconfig it seems
# if (UNIX AND NOT APPLE)
#   pkg_check_modules(PortMidi REQUIRED IMPORTED_TARGET portmidi)
# endif()

if (APPLE)
  add_library(portmidi INTERFACE)
  target_link_options(portmidi INTERFACE -L/usr/local/lib)
  target_link_libraries(portmidi INTERFACE libportmidi.dylib)
endif()

add_executable(clap-host
  application.cc
  application.hh
  audio-settings.cc
  audio-settings.hh
  audio-settings-widget.cc
  audio-settings-widget.hh

  plugin-param.cc
  plugin-param.hh
  plugin-host.cc
  plugin-host.hh
  plugin-quick-control-widget.cc
  plugin-quick-control-widget.hh
  plugin-quick-controls-widget.cc
  plugin-quick-controls-widget.hh

  CMakeLists.txt
  device-reference.hh
  engine.cc
  engine.hh
  main.cc
  main-window.cc
  main-window.hh
  midi-settings.cc
  midi-settings.hh
  midi-settings-widget.cc
  midi-settings-widget.hh
  plugin-info.hh
  plugin-parameters-widget.cc
  plugin-parameters-widget.hh
  settings.cc
  settings-dialog.cc
  settings-dialog.hh
  settings.hh
  settings-widget.cc
  settings-widget.hh
  )

set_target_properties(clap-host PROPERTIES CXX_STANDARD 17)
target_link_libraries(clap-host Qt6::Widgets Qt6::Core)
target_link_libraries(clap-host portmidi portaudio)

if (LINUX)
  target_link_libraries(clap-host dl pthread)
endif()

if (APPLE)
  set_target_properties(clap-host PROPERTIES OSX_ARCHITECTURES x86_64)

  find_library(CORE_FOUNDATION CoreFoundation)
  find_library(CORE_AUDIO CoreAudio)
  find_library(CORE_AUDIO CoreServices)
  find_library(CORE_MIDI CoreMIDI)
  find_library(AUDIO_UNIT AudioUnit)
  find_library(AUDIO_TOOLBOX AudioToolbox)
  find_library(CARBON Carbon)
  target_link_libraries(clap-host ${CARBON} ${AUDIO_UNIT} ${AUDIO_TOOLBOX} ${CORE_MIDI} ${CORE_AUDIO} ${CORE_SERVICES} ${CORE_FOUNDATION})
endif()

install(TARGETS clap-host DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")