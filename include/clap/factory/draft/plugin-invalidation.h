#pragma once

#include "../../private/std.h"
#include "../../private/macros.h"

// Use it to retrieve const clap_plugin_invalidation_factory_t* from
// clap_plugin_entry.get_factory()
static const CLAP_CONSTEXPR char CLAP_PLUGIN_INVALIDATION_FACTORY_ID[] =
   "clap.plugin-invalidation-factory/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_invalidation_source {
   // Directory containing the file(s) to scan, must be absolute
   const char *directory;

   // globing pattern, in the form *.dll
   const char *filename_glob;

   // should the directory be scanned recursively?
   bool recursive_scan;
} clap_plugin_invalidation_source_t;

// Used to figure out when a plugin needs to be scanned again.
// Imagine a situation with a single entry point: my-plugin.clap which then scans itself
// a set of "sub-plugins". New plugin may be available even if my-plugin.clap file doesn't change.
// This interfaces solves this issue and gives a way to the host to monitor additional files.
typedef struct clap_plugin_invalidation_factory {
   // Get the number of invalidation source.
   uint32_t(CLAP_ABI *count)(const struct clap_plugin_invalidation_factory *factory);

   // Get the invalidation source by its index.
   // [thread-safe]
   const clap_plugin_invalidation_source_t *(CLAP_ABI *get)(
      const struct clap_plugin_invalidation_factory *factory, uint32_t index);

   // In case the host detected a invalidation event, it can call refresh() to let the
   // plugin_entry update the set of plugins available.
   // If the function returned false, then the plugin needs to be reloaded.
   bool(CLAP_ABI *refresh)(const struct clap_plugin_invalidation_factory *factory);
} clap_plugin_invalidation_factory_t;

#ifdef __cplusplus
}
#endif
