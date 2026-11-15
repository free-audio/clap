#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_RESOURCE_DIRECTORY[] = "clap.resource-directory/1";

#ifdef __cplusplus
extern "C" {
#endif

/// @page Resource Directory
///
/// This extension provides a way for the plugin to store its resources as file in a directory
/// provided by the host and recover them later on.
///
/// The plugin **must** store relative path in its state toward resource directories.
///
/// Resource sharing:
/// - shared directory is shared among all plugin instances, hence mostly appropriate for read-only
/// content
///   -> suitable for read-only content
/// - exclusive directory is exclusive to the plugin instance
///   -> if the plugin, then its exclusive directory must be duplicated too
///   -> suitable for read-write content
///
/// Keeping the shared directory clean:
/// - to avoid clashes in the shared directory, plugins are encouraged to organize their files in
///   sub-folders, for example create one subdirectory using the vendor name
/// - don't use symbolic links or hard links which points outside of the directory
///
/// Resource life-time:
/// - exclusive folder content is managed by the plugin instance
/// - exclusive folder content is deleted when the plugin instance is removed from the project
/// - shared folder content isn't managed by the host, until all plugins using the shared directory
///   are removed from the project
///
/// Note for the host
/// - try to use the filesystem's copy-on-write feature when possible for reducing exclusive folder
///   space usage on duplication
/// - host can "garbage collect" the files in the shared folder using:
///     clap_plugin_resource_directory.get_files_count()
///     clap_plugin_resource_directory.get_file_path()
///   but be **very** careful before deleting any resources

typedef struct clap_plugin_resource_directory {
   // Sets the directory in which the plugin can save its resources.
   // The directory remains valid until it is overridden or the plugin is destroyed.
   // If path is null or blank, it clears the directory location.
   // path must be absolute.
   // [main-thread]
   void(CLAP_ABI *set_directory)(const clap_plugin_t *plugin, const char *path, bool is_shared);

   // Asks the plugin to put its resources into the resource directory.
   // It is not necessary to collect files which belongs to the plugin's
   // factory content unless the param all is true.
   // [main-thread]
   void(CLAP_ABI *collect)(const clap_plugin_t *plugin, bool all);

   // Returns the number of files used by the plugin in the shared resource folder.
   // [main-thread]
   uint32_t(CLAP_ABI *get_files_count)(const clap_plugin_t *plugin);

   // Retrieves relative file path to the resource directory.
   // @param path writable memory to store the path
   // @param path_size number of available bytes in path
   // Returns the number of bytes in the path, or -1 on error
   // [main-thread]
   int32_t(CLAP_ABI *get_file_path)(const clap_plugin_t *plugin,
                                    uint32_t             index,
                                    char                *path,
                                    uint32_t             path_size);
} clap_plugin_resource_directory_t;

typedef struct clap_host_resource_directory {
   // Request the host to setup a resource directory with the specified sharing.
   // Returns true if the host will perform the request.
   // [main-thread]
   bool(CLAP_ABI *request_directory)(const clap_host_t *host, bool is_shared);

   // Tell the host that the resource directory of the specified sharing is no longer required.
   // If is_shared = false, then the host may delete the directory content.
   // [main-thread]
   void(CLAP_ABI *release_directory)(const clap_host_t *host, bool is_shared);
} clap_host_resource_directory_t;

#ifdef __cplusplus
}
#endif
