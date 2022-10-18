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

enum {
   // Here the resource directory is only used by this plugin instance.
   CLAP_RESOURCE_DIRECTORY_SHARE_PER_INSTANCE,

   // Here the resource directory is shared among all plugins with the same vendor string.
   // This entry is forbidden if the vendor is blank or null.
   CLAP_RESOURCE_DIRECTORY_SHARE_PER_VENDOR,
};
typedef uint32_t clap_resource_directory_sharing;

typedef struct clap_plugin_resource_directory {
   // Returns true if the plugin wants a resource directory with the specified sharing.
   // [main-thread]
   bool(CLAP_ABI *wants_resource_directory)(const clap_plugin_t            *plugin,
                                            clap_resource_directory_sharing sharing);

   // Sets the directory in which the plugin can save its resources.
   // The directory remains valid until it is overriden or the plugin is destroyed.
   // If path is null or blank, it clears the directory location.
   //
   // [main-thread]
   void(CLAP_ABI *set_resource_directory)(const clap_plugin_t            *plugin,
                                          const char                     *path,
                                          clap_resource_directory_sharing sharing);

   // Asks the plugin to put its resources into the resources directory.
   // It is not necessary to collect files which belongs to the plugin's
   // factory content unless the param all is true.
   // [main-thread]
   void(CLAP_ABI *collect)(const clap_plugin_t *plugin, bool all);
} clap_plugin_resource_directory_t;

#ifdef __cplusplus
}
#endif
