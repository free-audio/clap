#pragma once

#include "private/macros.h"
#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)

typedef struct clap_plugin_invalidation_source {
   // Directory containing the file(s) to scan
   const char *directory;

   // globing pattern, in the form *.dll
   const char *filename_glob;

   // should the directory be scanned recursively?
   alignas(1) bool recursive_scan;
} clap_plugin_invalidation_source_t;

static const CLAP_CONSTEXPR char CLAP_PLUGIN_INVALIDATION_FACTORY_ID[] = "clap.plugin-invalidation-factory";

// Used to figure out when a plugin needs to be scanned again.
// Imagine a situation with a single entry point: my-plugin.clap which then scans itself
// a set of "sub-plugins". New plugin may be available even if my-plugin.clap file doesn't change.
// This interfaces solves this issue and gives a way to the host to monitor additional files.
typedef struct clap_plugin_invalidation_factory {
   // Get the number of invalidation source.
   uint32_t (*count)(const struct clap_plugin_invalidation_factory *factory);

   // Get the invalidation source by its index.
   // [thread-safe]
   const clap_plugin_invalidation_source_t *(*get)(
      const struct clap_plugin_invalidation_factory *factory, uint32_t index);

   // In case the host detected a invalidation event, it can call refresh() to let the
   // plugin_entry scan the set of plugins available.
   // If the function returned false, then the plugin needs to be reloaded.
   bool (*refresh)(const struct clap_plugin_invalidation_factory *factory);
} clap_plugin_invalidation_factory_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif