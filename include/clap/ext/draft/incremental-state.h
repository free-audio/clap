#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_INCREMENTAL_STATE[] = "clap.incremental-state.draft/0";

// This extension is useful for the host to collect incremental changes instead of saving the plugin
// state after each changes.
//
// The typical use case is for crash recovery:
// 1. the host saves the entire project (expensive)
// 2. many things, happens and the host saves little delta operations to disk (cheap)
// 3. the application crashes
//
// Then once the application is restarted, the user can decide to recover the project which will
// work as follow:
// 1. open the last project state
// 2. apply all the deltas in sequence
// 3. project is ready and should be in the same state as it was before the crash
//
// Saving a project, can be an expensive task especially for large project and can
// cause playback performance issues.
// This is why saving deltas is interesting because it is lightweight.
//
// This interface is not meant to replace saving the entire project.
//
// If the plugin decides to implement this interface, it should then be able to encode
// the deltas in a space efficient way.
//
// The plugin can assume that the delta will be applied on the same computer as the
// one used to produce it. Meaning that it doesn't have to take care of endianness
// and can store path to files on the computer.

typedef struct clap_plugin_incremental_state {
   // [main-thread]
   bool (*apply_delta)(clap_plugin_t *plugin, const char *data, size_t size);
} clap_plugin_incremental_state_t;

typedef struct clap_host_incremental_state {
   // Adds a delta, its size should be reasonably small.
   //
   // [main-thread]
   void (*add_delta)(clap_host_t *host, const char *data, size_t size);

   // One change happened which can't be encoded as a delta, and requires
   // the host to save the entire state again.
   //
   // [main-thread]
   void (*save_is_required)(clap_host_t *host);
} clap_host_incremental_state_t;
