#pragma once

#include "../clap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_GUI_WIN32 "clap/gui/win32"

// we don't want to include windows.h from this file.
typedef void *clap_hwnd;

struct clap_plugin_gui_win32 {
   // [main-thread]
   bool (*attach)(struct clap_plugin *plugin, clap_hwnd window);

   // [main-thread]
   bool (*detach)(struct clap_plugin *plugin);
};

#ifdef __cplusplus
}
#endif