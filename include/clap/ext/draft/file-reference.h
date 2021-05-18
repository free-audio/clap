#pragma once

#include "../../clap.h"
#include "../../hash.h"

#define CLAP_EXT_FILE_REFERENCE "clap/file-reference"

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
} clap_file_reference;

typedef struct clap_plugin_file_reference {
   // returns the number of file reference this plugin has
   // [main-thread]
   uint32_t (*count)(clap_plugin *plugin);

   // gets the file reference at index
   // returns true on success
   // [main-thread]
   bool (*get)(clap_plugin *plugin, uint32_t index, clap_file_reference *file_reference);

   // [main-thread]
   bool (*get_hash)(clap_plugin *plugin, clap_id resource_id, clap_hash hash, uint8_t *digest);

   // updates the path to a file reference
   // [main-thread]
   bool (*set)(clap_plugin *plugin, clap_id resource_id, const char *path);

   // [main-thread]
   bool (*save_resources)(clap_plugin *plugin);
} clap_plugin_file_reference;

typedef struct clap_host_file_reference {
   // informs the host that the file references have changed, the host should schedule a full rescan
   // [main-thread]
   void (*changed)(clap_host *host);

   void (*set_dirty)(clap_host *host , clap_id resource_id);
} clap_host_file_reference;

#ifdef __cplusplus
}
#endif
