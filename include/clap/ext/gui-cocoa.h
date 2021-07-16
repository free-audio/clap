#pragma once

#include "../clap.h"

static CLAP_CONSTEXPR const char CLAP_EXT_GUI_COCOA[] = "clap.gui-cocoa";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gui_cocoa {
   // [main-thread]
   bool (*attach)(const clap_plugin *plugin, void *nsView);
} clap_plugin_gui_cocoa;

#ifdef __cplusplus
}
#endif
