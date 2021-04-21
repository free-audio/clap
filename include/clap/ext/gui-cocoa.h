#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../clap.h"

#define CLAP_EXT_GUI_COCOA "clap/gui/cocoa"

struct clap_plugin_embed_cocoa {
   // [main-thread]
   bool (*attach)(struct clap_plugin *plugin, void *nsView);

   // [main-thread]
   bool (*detach)(struct clap_plugin *plugin);
};

#ifdef __cplusplus
}
#endif
