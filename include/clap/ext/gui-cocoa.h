#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../clap.h"

#define CLAP_EXT_GUI_COCOA "clap/gui-cocoa"

typedef struct clap_plugin_gui_cocoa {
   // [main-thread]
   bool (*attach)(const clap_plugin *plugin, void *nsView);
} clap_plugin_gui_cocoa;

#ifdef __cplusplus
}
#endif
