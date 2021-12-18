#pragma once

#include "plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_PLUGIN_FACTORY_ID "clap.plugin-factory"

// This interface is the entry point of the dynamic library.
//
// There is an invalidation mechanism for the set of plugins which is based on files.
// The host can watch the plugin DSO's mtime and a set of files's mtime provided by
// get_clap_invalidation_source().
//
// The set of plugins must not change, except during a call to refresh() by the host.
//
// Every methods must be thread-safe.
struct clap_plugin_factory {
   /* Get the number of plugins available.
    * [thread-safe] */
   uint32_t (*get_plugin_count)(void);

   /* Retrieves a plugin descriptor by its index.
    * Returns null in case of error.
    * The descriptor does not need to be freed.
    * [thread-safe] */
   const clap_plugin_descriptor *(*get_plugin_descriptor)(uint32_t index);

   /* Create a clap_plugin by its plugin_id.
    * The returned pointer must be freed by calling plugin->destroy(plugin);
    * The plugin is not allowed to use the host callbacks in the create method.
    * Returns null in case of error.
    * [thread-safe] */
   const clap_plugin *(*create_plugin)(const clap_host *host, const char *plugin_id);
};

#ifdef __cplusplus
}
#endif