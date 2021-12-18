#pragma once

#include "version.h"
#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_entry {
    clap_version clap_version; // initialized to CLAP_VERSION

   bool (*init)(const char *plugin_path);
   void (*deinit)(void);

    const void (*get_factory)(const char *factory_id);
} clap_plugin_entry;

/* Entry point */
CLAP_EXPORT extern const struct clap_plugin_entry clap_plugin_entry;

#ifdef __cplusplus
}
#endif
