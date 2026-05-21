#pragma once

#include "../../plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @page background activation
/// @brief Activate and Deactivate from a background thread.
///
/// Some plugin needs to perform complex computation or even I/O during activation
/// and this blocks the main thread.
///
/// This extension is here to offer an alternative way to activate the plugin from a background
/// thread, to keep the main-thread running.
///
/// Background activation must not be concurrent to a plugin activation/deactivation on the
/// main-thread.
///
/// Implementing this extension implies that background activation and deactivation
/// are beneficial and preferred for this plugin.

static CLAP_CONSTEXPR const char CLAP_EXT_BACKGROUND_ACTIVATION[] = "clap.background-activation/1";

typedef struct clap_plugin_background_activation {
   // Same as clap_plugin.activate() but it is called from a background thread.
   // See clap_host_background_progress for progress feedback and cancelation.
   //
   // [background-thread]
   bool(CLAP_ABI *activate_from_background_thread)(clap_plugin_t *plugin,
                                                   double         sample_rate,
                                                   uint32_t       min_frames_count,
                                                   uint32_t       max_frames_count);

   // Same as clap_plugin.deactivate() but it is called from a background thread.
   // See clap_host_background_progress for progress feedback and cancelation.
   //
   // [background-thread]
   void(CLAP_ABI *deactivate_from_background_thread)(const struct clap_plugin *plugin);
} clap_plugin_background_activation_t;

#ifdef __cplusplus
}
#endif
