#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_GUI_X11[] = "clap.gui-x11";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_plugin_gui_x11 {
   // Use the protocol XEmbed
   // https://specifications.freedesktop.org/xembed-spec/xembed-spec-latest.html
   // [main-thread]
   bool (*attach)(const clap_plugin_t *plugin, const char *display_name, unsigned long window);
} clap_plugin_gui_x11_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif