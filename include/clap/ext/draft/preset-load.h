#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_PRESET_LOAD "clap/draft/preset-load"

typedef struct clap_plugin_preset_load {
   // Loads a preset in the plugin native preset file format from a path.
   // [main-thread]
   bool (*from_file)(const clap_plugin *plugin, const char *path);
} clap_plugin_preset_load;

#ifdef __cplusplus
}
#endif