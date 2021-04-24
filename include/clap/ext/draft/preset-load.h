#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_PRESET_LOAD "clap/draft/preset-load"

struct clap_plugin_preset_load {
   // Loads a preset in the plugin native preset file format from a path.
   // [main-thread]
   bool (*load_from_file)(struct clap_plugin *plugin, const char *path);
};

#ifdef __cplusplus
}
#endif