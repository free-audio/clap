#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "macros.h"

static const CLAP_CONSTEXPR char CLAP_PLUGIN_INVALIDATION_ID[] = "clap.plugin-invalidation";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_invalidation_source {
   // Directory containing the file(s) to scan
   const char *directory;

   // globing pattern, in the form *.dll
   const char *filename_glob;

   // should the directory be scanned recursively?
   bool recursive_scan;
} clap_plugin_invalidation_source;

// Used to figure out when a plugin needs to be scanned again.
// Imagine a situation with a single entry point: my-plugin.clap which then scans itself
// a set of "sub-plugins". New plugin may be available even if my-plugin.clap file doesn't change.
// This interfaces solves this issue and gives a way to the host to monitor additional files.
struct clap_plugin_invalidation_factory {
   // Get the number of invalidation source.
   uint32_t (*count)(void);

   // Get the invalidation source by its index.
   // [thread-safe]
   const clap_plugin_invalidation_source *(*get)(uint32_t index);

   // In case the host detected a invalidation event, it can call refresh() to let the
   // plugin_entry scan the set of plugins available.
   void (*refresh)(void);
};

#ifdef __cplusplus
}
#endif