# Changes in 1.0.3

* Fix an inconsistence in `clap_plugin->destroy()` documentation:
  it is now **required** to deactivate the plugin before destroying it.
* Improve documentation for `clap_host_params->request_flush()`.

# Changes in 1.0.2

* CMake: add `CLAP_BUILD_TESTS` which enables the tests.
* Fixes compilation for Visual Studio 2019 and GCC9.

# Changes in 1.0.1

* [gui.h](include/clap/ext/gui.h): fix doc: set_scale must be provided
* [events.h](include/clap/events.h): remove `clap_event_type` which was never used
* [draft/transport-control.h](include/clap/ext/draft/transport-control.h): rename from `CLAP_EXT_CV` to `CLAP_EXT_TRANSPORT_CONTROL`
* [draft/tuning.h](include/clap/ext/draft/tuning.h): rename `clap_client_tuning` to `clap_plugin_tuning`
* [macros.h](include/clap/private/macros.h): fix compatibility with C17

# Changes in 1.0.0

## New stable interfaces

* [audio-ports-config.h](include/clap/ext/audio-ports-config.h)
* [audio-ports.h](include/clap/ext/audio-ports.h)
* [event-registry.h](include/clap/ext/event-registry.h)
* [gui.h](include/clap/ext/gui.h)
* [latency.h](include/clap/ext/latency.h)
* [log.h](include/clap/ext/log.h)
* [note-name.h](include/clap/ext/note-name.h)
* [note-ports.h](include/clap/ext/note-ports.h)
* [params.h](include/clap/ext/params.h)
* [posix-fd-support.h](include/clap/ext/posix-fd-support.h)
* [render.h](include/clap/ext/render.h)
* [state.h](include/clap/ext/state.h)
* [tail.h](include/clap/ext/tail.h)
* [thread-check.h](include/clap/ext/thread-check.h)
* [thread-pool.h](include/clap/ext/thread-pool.h)
* [timer-support.h](include/clap/ext/time-support.h)

## New draft interfaces

* [ambisonic.h](include/clap/ext/draft/ambisonic.h)
* [check-for-update.h](include/clap/ext/draft/check-for-update.h)
* [cv.h](include/clap/ext/draft/cv.h)
* [file-reference.h](include/clap/ext/draft/file-reference.h)
* [midi-mappings.h](include/clap/ext/draft/midi-mappings.h)
* [preset-load.h](include/clap/ext/draft/preset-load.h)
* [quick-controls.h](include/clap/ext/draft/quick-controls.h)
* [surround.h](include/clap/ext/draft/surround.h)
* [track-info.h](include/clap/ext/draft/track-info.h)
* [transport-control.h](include/clap/ext/draft/transport-control.h)
* [tuning.h](include/clap/ext/draft/tuning.h)
* [voice-info.h](include/clap/ext/draft/voice-info.h)
