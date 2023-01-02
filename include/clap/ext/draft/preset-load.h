#pragma once

#include "../../plugin.h"

static const char CLAP_EXT_PRESET_LOAD[] = "clap.preset-load.draft/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_preset_load {
   // Loads a preset in the plugin native preset file format from a path.
   // [main-thread]
   bool(CLAP_ABI *from_file)(const clap_plugin_t *plugin, const char *path);

   // Loads a preset in the plugin native preset file format from a path which points to a preset
   // container file.
   // preset_id must be a valid string, which is specific to the plugin itself and identifies the
   // desired preset within the given preset container.
   //
   // [main-thread]
   bool(CLAP_ABI *from_container_file)(const clap_plugin_t *plugin,
                                       const char          *path,
                                       const char          *preset_id);
} clap_plugin_preset_load_t;

#ifdef __cplusplus
}
#endif
