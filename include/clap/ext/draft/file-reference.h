#pragma once

#include "../../plugin.h"
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
///
/// Regarding the hash algorithm to choose, there are multiple options yet as of 2022,
/// BLAKE3 seems to be the best choice in regards to performances and robustness while
/// also providing a very small pure C library with permissive licensing.
/// That's why we encourage plugins and hosts to support BLAKE3.

typedef struct clap_file_reference {
   clap_id resource_id;
   bool    belongs_to_plugin_collection;

   size_t path_capacity; // [in] the number of bytes reserved in path
   size_t path_size;     // [out] the actual length of the path, can be bigger than path_capacity
   char  *path; // path to the file on the disk, must be null terminated, and maybe truncated if the
                // capacity is less than the size
} clap_file_reference_t;

typedef struct clap_plugin_file_reference {
   // returns the number of file reference this plugin has
   // [main-thread]
   uint32_t (*count)(const clap_plugin_t *plugin);

   // gets the file reference at index
   // returns true on success
   // [main-thread]
   bool (*get)(const clap_plugin_t *plugin, uint32_t index, clap_file_reference_t *file_reference);

   // This method does not compute the hash.
   // It is only used in case of missing resource. The host may have additionnal known resource
   // location and may be able to locate the file by using its known hash.
   // [main-thread]
   bool (*get_hash)(const clap_plugin_t *plugin,
                    clap_id              resource_id,
                    clap_hash            hash,
                    uint8_t             *digest,
                    uint32_t             digest_size);

   // updates the path to a file reference
   // [main-thread]
   bool (*update_path)(const clap_plugin_t *plugin, clap_id resource_id, const char *path);

   // [main-thread]
   bool (*save_resources)(const clap_plugin_t *plugin);
} clap_plugin_file_reference_t;

typedef struct clap_host_file_reference {
   // informs the host that the file references have changed, the host should schedule a full rescan
   // [main-thread]
   void (*changed)(const clap_host_t *host);

   // [main-thread]
   void (*set_dirty)(const clap_host_t *host, clap_id resource_id);
} clap_host_file_reference;

#ifdef __cplusplus
}
#endif
