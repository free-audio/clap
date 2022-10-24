#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_RESOURCE_DIRECTORY[] = "clap.resource-directory.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

/// @page Resource Directory
///
/// This extension provides a way for the plugin to store its resources as file in a directory
/// provided by the host and recover them later on.
///
/// Resource sharing:
/// - shared directory is shared among all plugin instances
/// - exclusive directory is exclusive to the plugin instance, which means that if the plugin
///   is duplicated, its exclusive directory must be duplicated too
///
/// Keeping the shared directory clean:
/// - to avoid clashes in the shared directory, plugins are encourraged to organize their files in
///   sub-folders, for example create one subdirectory using the vendor name

typedef struct clap_plugin_resource_directory {
   // Returns true if the plugin wants a resource directory with the specified sharing.
   // [main-thread]
   bool(CLAP_ABI *wants_directory)(const clap_plugin_t *plugin, bool is_shared);

   // Sets the directory in which the plugin can save its resources.
   // The directory remains valid until it is overriden or the plugin is destroyed.
   // If path is null or blank, it clears the directory location.
   //
   // [main-thread]
   void(CLAP_ABI *set_directory)(const clap_plugin_t *plugin,
                                          const char          *path,
                                          bool                 is_shared);

   // Asks the plugin to put its resources into the resources directory.
   // It is not necessary to collect files which belongs to the plugin's
   // factory content unless the param all is true.
   // [main-thread]
   void(CLAP_ABI *collect)(const clap_plugin_t *plugin, bool all);
} clap_plugin_resource_directory_t;

typedef struct clap_host_resource_directory {
   // Request the host to setup a resource directory with the specified sharing.
   // Returns true if the host will perform the request.
   //
   // [main-thread]
   bool(CLAP_ABI *request_directory)(const clap_host *host, bool is_shared);

   // Tell the host that the resource directory of the specified sharing is no longer required.
   // If is_shared = false, then the host may delete the directory content.
   //
   // [main-thread]
   void(CLAP_ABI *release_directory)(const clap_host *host, bool is_shared);
} clap_host_resource_directory_t;

#ifdef __cplusplus
}
#endif
