#pragma once

#include "../clap.h"

#define CLAP_EXT_GUI_X11 "clap/gui/x11"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gui_x11 {
   // Use the protocol XEmbed
   // [main-thread]
   bool (*attach)(clap_plugin * plugin,
                  const char *  display_name,
                  unsigned long window);

   // [main-thread]
   bool (*detach)(clap_plugin *plugin);
} clap_plugin_gui_x11;

#ifdef __cplusplus
}
#endif