#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_FILE_REFERENCE[] = "clap.file-reference.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

/// @page File Reference
///
/// This extension provides a way for the host to know about files which are used
/// by the plugin, like a wavetable, a sample, ...
///
/// The host can then:
/// - collect and save
/// - search for missing files by using:
///   - filename
///   - hash
///   - file size
/// - be aware that some external file references are marked as dirty and need to be saved.
///
/// Regarding the hashing algorithm, as of 2022 BLAKE3 seems to be the best choice in regards to
/// performances and robustness while also providing a very small pure C library with permissive
/// licensing. For more info see https://github.com/BLAKE3-team/BLAKE3
///
/// This extension only exposes one hashing algorithm on purpose.

// This describes a file currently used by the plugin
typedef struct clap_file_reference {
   clap_id resource_id;

   // Flag indicating that the plugin may be able to (re-)install a collection that provides
   // this resource. DAWs can provide a user option to ignore or include this resource during
   // "collect and save".
   bool    belongs_to_plugin_collection;

   size_t path_capacity; // [in] the number of bytes reserved in path
   size_t path_size;     // [out] the actual length of the path, can be bigger than path_capacity
   char  *path; // [in,out] absolute path to the file on the disk, must be null terminated, and
                // may be truncated if the capacity is less than the size
} clap_file_reference_t;

typedef struct clap_plugin_file_reference {
   // Returns the number of file reference this plugin has
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Gets the file reference at index
   // returns true on success
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t   *plugin,
                       uint32_t               index,
                       clap_file_reference_t *file_reference);

   // This method can be called even if the file is missing.
   // So the plugin is encouraged to store the digest in its state.
   //
   // digest is an array of 32 bytes.
   //
   // [main-thread]
   bool(CLAP_ABI *get_blake3_digest)(const clap_plugin_t *plugin,
                                     clap_id              resource_id,
                                     uint8_t             *digest);

   // This method can be called even if the file is missing.
   // So the plugin is encouraged to store the file's size in its state.
   //
   // [main-thread]
   bool(CLAP_ABI *get_file_size)(const clap_plugin_t *plugin, clap_id resource_id, uint64_t *size);

   // Updates the path to a file reference
   // [main-thread]
   bool(CLAP_ABI *update_path)(const clap_plugin_t *plugin, clap_id resource_id, const char *path);

   // Request all pending changes to be flushed to disk (e.g. for destructive
   // sample editor plugins), needed during "collect and save".
   // [main-thread]
   bool(CLAP_ABI *save_resources)(const clap_plugin_t *plugin);
} clap_plugin_file_reference_t;

typedef struct clap_host_file_reference {
   // Informs the host that the file references have changed, the host should schedule a full rescan.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);

   // Informs the host that file contents have changed, a call to save_resources() is needed.
   // [main-thread]
   void(CLAP_ABI *set_dirty)(const clap_host_t *host, clap_id resource_id);
} clap_host_file_reference;

#ifdef __cplusplus
}
#endif
