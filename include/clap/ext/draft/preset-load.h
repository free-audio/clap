#pragma once

#include "../../plugin.h"

static const char CLAP_EXT_PRESET_LOAD[] = "clap.preset-load.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_preset_load {
   // Loads a preset in the plugin native preset file format from a path.
   // [main-thread]
   bool (*from_file)(const clap_plugin_t *plugin, const char *path);
} clap_plugin_preset_load_t;

#ifdef __cplusplus
}
#endif
