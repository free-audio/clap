#pragma once

#include "../state-context.h"

/// @page background state context
/// @brief Save and Load state from a background thread.
///
/// Some plugin needs to perform complex computation or even I/O during state loading or saving and
/// this blocks the main thread.
///
/// This extension is here to offer an alternative way to load or save the plugin plugin from a
/// background thread, to keep the main-thread running.
///
/// Background save and load must not be concurrent to main-thread save and load.
///
/// Implementing this extension implies that background state saving and loading
/// are beneficial and preferred for this plugin.

#ifdef __cplusplus
extern "C" {
#endif

static CLAP_CONSTEXPR const char CLAP_EXT_BACKGROUND_STATE_CONTEXT[] =
   "clap.background-state-context/1";

typedef struct clap_plugin_background_state_context {
   // Same as clap_plugin_state_context.save() but it is called from a background thread.
   // See clap_host_background_progress for progress feedback and cancelation.
   // [background-thread]
   bool(CLAP_ABI *save_from_background_thread)(const clap_plugin_t  *plugin,
                                               const clap_ostream_t *stream,
                                               uint32_t              context_type);

   // Same as clap_plugin_state_context.load() but it is called from a background thread.
   // See clap_host_background_progress for progress feedback and cancelation.
   // [background-thread]
   bool(CLAP_ABI *load_from_background_thread)(const clap_plugin_t  *plugin,
                                               const clap_istream_t *stream,
                                               uint32_t              context_type);
} clap_plugin_background_state_context_t;

#ifdef __cplusplus
}
#endif
