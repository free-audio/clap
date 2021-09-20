#pragma once

#include "../../clap.h"
#include "../../hash.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_FILE_REFERENCE[] = "clap.file-reference.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

/// @page File Reference
///
/// This extension provides a way for the host to know about files which are used
/// by the preset, like a wavetable, a sample, ...
///
/// The host can then:
/// - collect and save
/// - search for missing files by using:
///   - filename
///   - hash
/// - be aware that some external file references are marked as dirty
///   and needs to be saved.

typedef struct clap_file_reference {
   clap_id resource_id;
   char    path[CLAP_PATH_SIZE];
   bool    belongs_to_plugin_collection;
} clap_file_reference;

typedef struct clap_plugin_file_reference {
   // returns the number of file reference this plugin has
   // [main-thread]
   uint32_t (*count)(const clap_plugin *plugin);

   // gets the file reference at index
   // returns true on success
   // [main-thread]
   bool (*get)(const clap_plugin *plugin, uint32_t index, clap_file_reference *file_reference);

   // This method does not compute the hash.
   // It is only used in case of missing resource. The host may have additionnal known resource
   // location and may be able to locate the file by using its known hash.
   // [main-thread]
   bool (*get_hash)(const clap_plugin *plugin,
                    clap_id            resource_id,
                    clap_hash          hash,
                    uint8_t *          digest,
                    uint32_t           digest_size);

   // updates the path to a file reference
   // [main-thread]
   bool (*update_path)(const clap_plugin *plugin, clap_id resource_id, const char *path);

   // [main-thread]
   bool (*save_resources)(const clap_plugin *plugin);
} clap_plugin_file_reference;

typedef struct clap_host_file_reference {
   // informs the host that the file references have changed, the host should schedule a full rescan
   // [main-thread]
   void (*changed)(const clap_host *host);

   // [main-thread]
   void (*set_dirty)(const clap_host *host, clap_id resource_id);
} clap_host_file_reference;

#ifdef __cplusplus
}
#endif
