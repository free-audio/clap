#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"

/// @page File Paths
///
/// This extension lets the plugin list the files it loads by path, eg: the impulse response of a
/// convolver, the sample of a sampler, an SFZ file or an amp model, so that a host without the
/// plugin's GUI can read and set them.
///
/// Without it, such a path only lives in the plugin's state, which the host can't read or edit,
/// and the user has to open the plugin's GUI to choose a file. An embedded or headless host, a
/// control surface or a remote controller can instead offer its own file browser and hand the
/// chosen path to the plugin.
///
/// File paths are not parameters: they have no range, they can't be automated or modulated, they
/// are never sent as events and they never reach the audio thread through the host. Every call
/// happens on the main thread, and the plugin applies a new path the same way it applies one
/// chosen from its own GUI.
///
/// Values:
/// - a value is a null terminated UTF-8 absolute path, or an empty string for no file
/// - the plugin owns the values: before any set() a path holds the plugin's default, and after
///   clap_plugin_state.load() it holds the saved value; the host never has to provide one
/// - the values are part of the plugin's state: clap_plugin_state.save() stores them and
///   clap_plugin_state.load() restores them; how the plugin stores them in its state is its own
///   business, eg: relative to a directory of clap.resource-directory; after a state load the host
///   reads the values again, without waiting for value_changed()
/// - a host that keeps its own copy of a path, eg: relative to its project so the project can be
///   moved, may call set() after clap_plugin_state.load() to apply it
///
/// Dirty state: changing a path changes the plugin's state, as changing a parameter does.
/// - after a successful set() the host knows its project changed; the plugin doesn't call
///   clap_host_state.mark_dirty() or clap_host_file_paths.value_changed() for it
/// - when the plugin changes a path by itself, eg: from its GUI, or because loading the file failed
///   and it went back to the previous one, it calls clap_host_file_paths.value_changed() and
///   clap_host_state.mark_dirty()

static CLAP_CONSTEXPR const char CLAP_EXT_FILE_PATHS[] = "clap.file-paths/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_file_path_info {
   // Stable identifier of the path, it must never change. It lives in its own namespace: it may
   // overlap with the ids of clap.params.
   clap_id id;

   // Displayable name of what the path is for, eg: "Impulse Response", "Sample".
   char name[CLAP_NAME_SIZE];
} clap_file_path_info_t;

typedef struct clap_plugin_file_paths {
   // Returns the number of file paths.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Copies the info of the file path at index into info.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_plugin_t   *plugin,
                            uint32_t               index,
                            clap_file_path_info_t *info);

   // Copies the current value of the file path path_id into path, null terminated, truncated to
   // path_size - 1 bytes if it is longer.
   //
   // Returns the length of the whole value in bytes, without the null terminator, so the host can
   // call it again with a larger buffer; 0 if there is no file. Returns -1 if path_id is unknown.
   //
   // After a successful set(), this returns the value just set, even if the plugin is still
   // loading the file.
   //
   // Note: CLAP_PATH_SIZE is too small for a file path, see string-sizes.h.
   // [main-thread]
   int32_t(CLAP_ABI *get)(const clap_plugin_t *plugin,
                          clap_id              path_id,
                          char                *path,
                          uint32_t             path_size);

   // Copies into display a short, displayable text for the current value of path_id, eg: the file
   // name without its directory, or the name of the impulse response the file holds; empty if
   // there is no file. The value itself, as get() returns it, stays the full path.
   //
   // Returns false if path_id is unknown.
   // [main-thread]
   bool(CLAP_ABI *get_display)(const clap_plugin_t *plugin,
                               clap_id              path_id,
                               char                *display,
                               uint32_t             display_size);

   // Sets the value of the file path path_id. path is a null terminated UTF-8 absolute path, or an
   // empty string to clear it. It is only valid for the duration of the call; the plugin copies
   // it.
   //
   // This can be called while the plugin is active and processing. The plugin hands the change to
   // its audio thread without blocking it, as it does for a file chosen from its own GUI; the file
   // may be loaded later, on another thread.
   //
   // Returns true if the plugin took the value. Returns false, and keeps the previous value, if
   // path_id is unknown or if the plugin refuses the path, eg: a file it can't open or read. A
   // plugin which only finds out later that it can't load the file goes back to a value it can
   // use and calls clap_host_file_paths.value_changed().
   // [main-thread]
   bool(CLAP_ABI *set)(const clap_plugin_t *plugin, clap_id path_id, const char *path);
} clap_plugin_file_paths_t;

typedef struct clap_host_file_paths {
   // Informs the host that the list of file paths or their info changed. All the values may have
   // changed too. The host calls count(), get_info() and get() again.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);

   // Informs the host that the plugin changed the value of path_id by itself. The host calls get()
   // and get_display() again.
   // [main-thread]
   void(CLAP_ABI *value_changed)(const clap_host_t *host, clap_id path_id);
} clap_host_file_paths_t;

#ifdef __cplusplus
}
#endif
