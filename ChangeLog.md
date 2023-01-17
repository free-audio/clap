# Changes in 1.1.7

* Add a [factory](include/clap/factory) folder for better organization and move our factories there
* [params.h](include/clap/ext/params.h): fix typos
* CMake: disable C17 targets for CMake < 3.21
* [plugin-features.h](include/clap/plugin-features.h): adds `note-detector` category for plugins which converts audio to notes

## Draft extensions

* [context-menu.h](include/clap/ext/draft/context-menu.h): add "title" menu entry
* [preset-load.h](include/clap/ext/draft/preset-load.h): load from URI instead of path, making the extension more powerful
* [remote-controls.h](include/clap/ext/draft/remote-controls.h): distinguish between device pages and preset pages
* [audio-ports-activation.h](include/clap/ext/draft/audio-ports-activation.h): `set_active()` now returns bool instead of void, this helps catching problems earlier especially with invalid arguments
* [audio-ports-config.h](include/clap/ext/audio-ports-config.h): add new draft extension: `clap_plugin_audio_ports_config_info` which lets the host query detailed port information in a given configuration.
* [surround.h](include/clap/ext/draft/surround.h): add `config_id` parameter when fetching port info
* [ambisonic.h](include/clap/ext/draft/ambisonic.h): add `config_id` parameter when fetching port info

## Draft factories

* [preset-discovery.h](include/clap/factory/draft/preset-discovery.h): new factory which allows the host to index the plugin presets which are stored on disk.

# Changes in 1.1.6

* [version.h](include/clap/version.h) `CLAP_VERSION_LT` was backwards (comparing current with arg
  vs arg with current). Correct and enhance tests.

# Changes in 1.1.5

* [plugin.h](include/clap/plugin.h): clarify plugin state after init()
* [plugin.h](include/clap/plugin.h): clarify when it is allowed to call get_extension()
* [plugin.h](include/clap/plugin.h): advice for plugin id and version strings
* [host.h](include/clap/host.h): clarify when it is allowed to call get_extension()
* [CMakeLists.txt](CMakeLists.txt): the target `clap-test` now includes `clap-plugin-template`
* Remove UTF-8 BOM from a few files
* [plugin-template.c](src/plugin-template.c): add state impl and some comments
* [audio-ports-activation.h](include/clap/ext/draft/audio-ports-activation.h): improved documentation
* [version.h](include/clap/version.h):
  * Add a CLAP_VERSION_GE(maj,min,rev), _EQ and _LT macro.
  * Remove the uint32_t cast from CLAP_VERSION_MAJOR, _MINOR, and _REVISION macro, and introduce it to the CLAP_VERSION_INIT macro.
  * If you rely on these macros being a uint32_t or parse this header using external software, this may be a breaking change.

# Changes in 1.1.4

* CMake: update some targets to link against `clap` instead of `clap-core`
* [params.h](include/clap/ext/params.h): clarify parameter range change, fix documentation typos, add missing items
* [plugin.h](include/clap/plugin.h): clarify data lifetime in `process()`
* [audio-ports-config.h](include/clap/ext/audio-ports-config.h): clarify `select()`: if succeed, the host should rescan the audio ports

## Draft extensions

### New

* [audio-ports-activation.h](include/clap/ext/draft/audio-ports-activation.h): new draft extension which allows a host to inform a plugin whether an audio port is an active port in the host audio context, and allow the host to respond accordingly
* [context-menu.h](include/clap/ext/draft/context-menu.h): new draft extension which let the host and plugin exchange context menu entries and popup the menu
* [param-indication.h](include/clap/ext/draft/param-indication.h): new draft extension which let the host inform the plugin that a parameter is currently mapped to a physical controller
* [remote-controls.h](include/clap/ext/draft/remote-controls.h): new draft extension which replaces `quick-controls.h`
* [resource-directory.h](include/clap/ext/draft/resource-directory.h): new draft extension which lets the plugin save resources in a directory provided by the host
* [triggers.h](include/clap/ext/draft/triggers.h): new draft extension which exposes triggers to the host, triggers are data-less events

### Improved

* [track-info](include/clap/ext/draft/track-info.h): refine the draft extension

### Removed

* `file-reference.h`: removed in favor of [resource-directory.h](include/clap/ext/draft/resource-directory.h)
* `quick-controls.h`: removed in favor of [remote-controls.h](include/clap/ext/draft/remote-controls.h)

# Changes in 1.1.3

* CMake: generate CMake and pkg-config package files on install
* CMake: `clap-core` target is now deprecated, use `clap` target instead
* [plugin.h](include/clap/plugin.h): make feature list on clap_plugin_descriptor_t const
* [entry.h](include/clap/entry.h): fix bad location on windows: replaced `%CommonFilesFolder%/CLAP/` by `%COMMONPROGRAMFILES%\CLAP`

# Changes in 1.1.2

* [macros.h](include/clap/private/macros.h): add `CLAP_ABI` which defines the calling convention and use it everywhere
* [events.h](include/clap/events.h): clarify `clap_input_events.size()`
* [check-for-update.h](include/clap/ext/draft/check-for-update.h): fix typedef
* [file-reference.h](include/clap/ext/draft/file-reference.h): improve documentation
* [params.h](include/clap/ext/params.h): clarify how the cookie works and add some notes about `flush()`
* [process.h](include/clap/process.h): clarify how the audio buffer mapping works
* [gui.h](include/clap/ext/gui.h): clarify `clap_plugin_gui.get_preferred_api()`
* [plugin-factory.h](include/clap/plugin-factory.h): mention `clap_plugin_invalidation_factory` which can be use to invalidate cached entries

# Changes in 1.1.1

* [clap.h](include/clap/clap.h): missing include for [state-context.h](include/clap/ext/draft/state-context.h).

# Changes in 1.1.0

* [state-context.h](include/clap/ext/draft/state-context.h): save/load state for duplicate or preset.
* [std.h](include/clap/private/std.h): fix compatibility with C++98.
* [note-name.h](include/clap/ext/note-name.h): missing typedef `clap_plugin_note_name_t`.
* [params.h](include/clap/ext/params.h): clarify `request_flush()` documentation.
* [thread-check.h](include/clap/ext/thread-check.h): clarify documentation.
* [voice-info.h](include/clap/ext/voice-info.h): promote `voice-info` out of draft.

# Changes in 1.0.3

* [plugin.h](include/clap/plugin.h): fix an inconsistency in `clap_plugin->destroy()` documentation:
  it is now **required** to deactivate the plugin before destroying it.
* [params.h](include/clap/ext/params.h): improve documentation for `clap_host_params->request_flush()`.
* [entry.h](include/clap/entry.h): improve documentation regarding `init()`, `deinit()` and CLAP search path.
* [gui.h](inclued/clap/gui.h): fix typo `clap_gui_resize_hints.preserve_aspect_ratio`
* [plugin-template](src/plugin-template.c): missing impl of plugin destroy.
* various documentation improvements

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
