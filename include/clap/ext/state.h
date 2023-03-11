#pragma once

#include "../plugin.h"
#include "../stream.h"

/// @page State
/// @brief state management
///
/// Plugins can implement this extension to save and restore both parameter
/// values and non-parameter state. This is used to persist a plugin's state
/// between project reloads, when duplicating and copying plugin instances, and
/// for host-side preset management.

static CLAP_CONSTEXPR const char CLAP_EXT_STATE[] = "clap.state";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_state {
   // Saves the plugin state into stream.
   // Returns true if the state was correctly saved.
   // [main-thread]
   bool(CLAP_ABI *save)(const clap_plugin_t *plugin, const clap_ostream_t *stream);

   // Loads the plugin state from stream.
   // Returns true if the state was correctly restored.
   // [main-thread]
   bool(CLAP_ABI *load)(const clap_plugin_t *plugin, const clap_istream_t *stream);
} clap_plugin_state_t;

typedef struct clap_host_state {
   // Tell the host that the plugin state has changed and should be saved again.
   // If a parameter value changes, then it is implicit that the state is dirty.
   // [main-thread]
   void(CLAP_ABI *mark_dirty)(const clap_host_t *host);
} clap_host_state_t;

#ifdef __cplusplus
}
#endif
