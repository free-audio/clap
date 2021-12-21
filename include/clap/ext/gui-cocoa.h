#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_GUI_COCOA[] = "clap.gui-cocoa";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_plugin_gui_cocoa {
   // [main-thread]
   bool (*attach)(const clap_plugin_t *plugin, void *nsView);
} clap_plugin_gui_cocoa_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
