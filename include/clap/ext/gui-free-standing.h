#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_GUI_FREE_STANDING[] = "clap.gui-free-standing";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_plugin_gui_free_standing {
   // Opens the plugin window as a free standing window, which means it is not
   // embedded in the host and managed by the plugin.
   // [main-thread]
   bool (*open)(const clap_plugin_t * plugin);
} clap_plugin_gui_free_standing_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif