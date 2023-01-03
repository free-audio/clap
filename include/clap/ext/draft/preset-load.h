#pragma once

#include "../../plugin.h"

static const char CLAP_EXT_PRESET_LOAD[] = "clap.preset-load.draft/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_preset_load {
   // Loads a preset in the plugin native preset file format from a URI. eg:
   // - "file:///home/abique/.u-he/Diva/Presets/Diva/HS Bass Nine.h2p"
   // - "file:///home/abique/my-sound-bank/<preset-id>"
   // - "plugin://<plugin-id>/<preset-id>"
   //
   // [main-thread]
   bool(CLAP_ABI *from_uri)(const clap_plugin_t *plugin, const char *uri);
} clap_plugin_preset_load_t;

#ifdef __cplusplus
}
#endif
