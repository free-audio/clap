#pragma once

#include "version.h"
#include "private/macros.h"

#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

// This interface is the entry point of the dynamic library.
//
// CLAP plugins standard search path:
//
// Linux
//   - /usr/lib/clap/
//   - ~/.clap
//     `-> ~/.local/lib/clap/ is considered, see https://github.com/free-audio/clap/issues/46
//
// Windows
//   - %CommonFilesFolder%/CLAP/
//   - %LOCALAPPDATA%/Programs/Common/VST3/
//
// MacOS
//   - /Library/Audio/Plug-Ins/CLAP
//   - ~/Library/Audio/Plug-Ins/CLAP
//
// Every methods must be thread-safe.
typedef struct clap_plugin_entry {
   clap_version_t clap_version; // initialized to CLAP_VERSION

   // This function must be called fist, and can only be called once.
   //
   // It should be as fast as possible, in order to perform very quick scan of the plugin
   // descriptors.
   //
   // It is forbidden to display graphical user interface in this call.
   // It is forbidden to perform user inter-action in this call.
   //
   // If the initialization depends upon expensive computation, maybe try to do them ahead of time
   // and cache the result.
   bool (*init)(const char *plugin_path);

   // No more calls into the DSO must be made after calling deinit().
   void (*deinit)(void);

   // Get the pointer to a factory.
   // See plugin-factory.h, vst2-converter.h ...
   //
   // Returns null if the factory is not provided.
   // The returned pointer must *not* be freed by the caller.
   const void *(*get_factory)(const char *factory_id);
} clap_plugin_entry_t;

/* Entry point */
CLAP_EXPORT extern const clap_plugin_entry_t clap_entry;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif