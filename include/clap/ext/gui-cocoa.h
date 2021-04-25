#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../clap.h"

#define CLAP_EXT_GUI_COCOA "clap/gui/cocoa"

typedef struct clap_plugin_embed_cocoa {
   // [main-thread]
   bool (*attach)(clap_plugin *plugin, void *nsView);

   // [main-thread]
   bool (*detach)(clap_plugin *plugin);
} clap_plugin_embed_cocoa;

#ifdef __cplusplus
}
#endif
