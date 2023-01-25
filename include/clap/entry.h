#pragma once

#include "version.h"
#include "private/macros.h"

#ifdef __cplusplus
extern "C" {
#endif

// This interface is the entry point of the dynamic library.
//
// CLAP plugins standard search path:
//
// Linux
//   - ~/.clap
//   - /usr/lib/clap
//
// Windows
//   - %COMMONPROGRAMFILES%\CLAP
//   - %LOCALAPPDATA%\Programs\Common\CLAP
//
// MacOS
//   - /Library/Audio/Plug-Ins/CLAP
//   - ~/Library/Audio/Plug-Ins/CLAP
//
// In addition to the OS-specific default locations above, a CLAP host must query the environment
// for a CLAP_PATH variable, which is a list of directories formatted in the same manner as the host
// OS binary search path (PATH on Unix, separated by `:` and Path on Windows, separated by ';', as
// of this writing).
//
// Each directory should be recursively searched for files and/or bundles as appropriate in your OS
// ending with the extension `.clap`.
//
// Every method must be thread-safe.
typedef struct clap_plugin_entry {
   clap_version_t clap_version; // initialized to CLAP_VERSION

   // This function must be called first, and can only be called once.
   //
   // It should be as fast as possible, in order to perform a very quick scan of the plugin
   // descriptors.
   //
   // It is forbidden to display graphical user interface in this call.
   // It is forbidden to perform user interaction in this call.
   //
   // If the initialization depends upon expensive computation, maybe try to do them ahead of time
   // and cache the result.
   //
   // If init() returns false, then the host must not call deinit() nor any other clap
   // related symbols from the DSO.
   bool(CLAP_ABI *init)(const char *plugin_path);

   // No more calls into the DSO must be made after calling deinit().
   void(CLAP_ABI *deinit)(void);

   // Get the pointer to a factory. See factory/plugin-factory.h for an example.
   //
   // Returns null if the factory is not provided.
   // The returned pointer must *not* be freed by the caller.
   const void *(CLAP_ABI *get_factory)(const char *factory_id);
} clap_plugin_entry_t;

/* Entry point */
CLAP_EXPORT extern const clap_plugin_entry_t clap_entry;

#ifdef __cplusplus
}
#endif
