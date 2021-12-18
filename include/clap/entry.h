#pragma once

#include "version.h"
#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

// This interface is the entry point of the dynamic library.
//
// Every methods must be thread-safe.
typedef struct clap_plugin_entry {
   clap_version clap_version; // initialized to CLAP_VERSION

   // Must be called fist
   bool (*init)(const char *plugin_path);

   // No more calls into the DSO must be made after calling deinit().
   void (*deinit)(void);

   // Get the pointer to a factory.
   // See plugin-factory.h, vst2-converter.h ...
   const void *(*get_factory)(const char *factory_id);
} clap_plugin_entry;

/* Entry point */
CLAP_EXPORT extern const clap_plugin_entry clap_entry;

#ifdef __cplusplus
}
#endif
