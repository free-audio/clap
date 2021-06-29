#pragma once

#include "../clap.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char CLAP_EXT_GUI_WIN32[] = "clap/gui-win32";

// we don't want to include windows.h from this file.
typedef void *clap_hwnd;

typedef struct clap_plugin_gui_win32 {
   // [main-thread]
   bool (*attach)(const clap_plugin *plugin, clap_hwnd window);
} clap_plugin_gui_win32;

#ifdef __cplusplus
}
#endif