#pragma once

#include "../clap.h"

static CLAP_CONSTEXPR const char CLAP_EXT_GUI_X11[] = "clap.gui-x11";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gui_x11 {
   // Use the protocol XEmbed
   // [main-thread]
   bool (*attach)(const clap_plugin * plugin,
                  const char *  display_name,
                  unsigned long window);
} clap_plugin_gui_x11;

#ifdef __cplusplus
}
#endif