#pragma once

#include "../../preloader.h"

static CLAP_CONSTEXPR const char CLAP_EXT_PRELOADER[] = "clap.preloader/1";

#ifdef __cplusplus
extern "C" {
#endif

/// Allows existing plugin instances to interact with Preloaders.
///
/// See "preloader.h" for a detailed description of the Snapshot and Commit operations.
///
/// Plugin types which do not support Preloaders at all should not implement this extension.
typedef struct clap_plugin_preloader {
   /// Snapshots a plugin's state to a new Preloader instance.
   ///
   /// If this succeeds, the caller receives ownership of the returned Preloader instance, which it
   /// must free using `clap_preloader.destroy`.
   ///
   /// The preloader is not allowed to use any of the provided `preloader_host` callbacks in the
   /// `create` call.
   ///
   /// Returns NULL if this fails.
   // [main-thread]
   const clap_preloader_t *(CLAP_ABI *snapshot_to_new)(const clap_plugin_t         *plugin,
                                                       const clap_preloader_host_t *preloader_host);

   /// Snapshots a plugin's state to an existing Preloader instance.
   ///
   /// This function must *not* be called concurrently to any other function using the same
   /// `clap_preloader` instance, including (but not limited to) any [preloader-thread] function.
   ///
   /// Returns `true` if the Snapshot operation succeeded, or `false` otherwise.
   // [main-thread]
   bool(CLAP_ABI *snapshot)(const clap_plugin_t *plugin, const clap_preloader_t *preloader);

   /// Commits a Preloader's state into this plugin instance.
   ///
   /// This function must *not* be called concurrently to any other function using the same
   /// `clap_preloader` instance, including (but not limited to) any [preloader-thread] function.
   ///
   /// Returns `true` if the Snapshot operation succeeded, or `false` otherwise.
   // [main-thread]
   bool(CLAP_ABI *commit)(const clap_plugin_t *plugin, const clap_preloader_t *preloader);
} clap_preloader_instantiate_t;

#ifdef __cplusplus
}
#endif
