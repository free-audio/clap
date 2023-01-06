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
   bool(CLAP_ABI *load)(const clap_plugin_t *plugin, const char *uri);
} clap_plugin_preset_load_t;

typedef struct clap_host_preset_load {
   // Called if clap_plugin_preset_load.load() failed.
   // os_error: the operating system error, if applicable. If not applicable set it to a non-error
   // value, eg: 0 on unix and Windows.
   //
   // [main-thread]
   void(CLAP_ABI *on_error)(const clap_plugin_t *plugin,
                            const char          *uri,
                            int32_t              os_error,
                            const char          *msg);

   // Informs the host that the following preset has been loaded.
   // This contributes to keep in sync the host preset browser and plugin preset browser.
   //
   // [main-thread]
   void(CLAP_ABI *loaded)(const clap_plugin_t *plugin, const char *uri);
} clap_host_preset_load_t;

#ifdef __cplusplus
}
#endif
