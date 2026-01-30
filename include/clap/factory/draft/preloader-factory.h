#pragma once

#include "../../private/macros.h"
#include "../../preloader.h"

static const CLAP_CONSTEXPR char CLAP_PLUGIN_PRELOADER_FACTORY_ID[] =
   "clap.plugin-preloader-factory/1";

#ifdef __cplusplus
extern "C" {
#endif

/// A Factory to create Preloaders.
typedef struct clap_preloader_factory {
   /// Returns `true` if a plugin type (identified by the given `plugin_id`) supports creating
   /// a Preloader by using `clap_preloader_factory.create`.
   ///
   /// If this returns `false`, then using `clap_preloader_factory.create` with the same `plugin_id`
   /// will always fail.
   // [thread-safe]
   bool(CLAP_ABI *plugin_supports_preloader)(const struct clap_preloader_factory *factory,
                                             const char                          *plugin_id);

   /// Creates a new Preloader for a given plugin type identified by the given `plugin_id`.
   ///
   /// If this succeeds, the caller receives ownership of the returned Preloader instance, which it
   /// must free using `clap_preloader.destroy`.
   ///
   /// The preloader is not allowed to use any of the provided `preloader_host` callbacks in the
   /// `create` call.
   ///
   /// Returns NULL if this fails, or if the given plugin type does not support Preloaders.
   // [thread-safe]
   const clap_preloader_t *(CLAP_ABI *create)(const struct clap_preloader_factory *factory,
                                              const clap_preloader_host_t         *preloader_host,
                                              const char                          *plugin_id);
} clap_plugin_preloader_factory_t;

#ifdef __cplusplus
}
#endif
