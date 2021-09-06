#pragma once

#include "../clap.h"

static CLAP_CONSTEXPR const char CLAP_EXT_RENDER[] = "clap.render";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // Default setting, used to play "realtime"
   CLAP_RENDER_REALTIME = 0,

   // Used while rendering the song; no realtime pressure
   CLAP_RENDER_OFFLINE = 1,
};
typedef int32_t clap_plugin_render_mode;

// The render extension is used to let the plugin know if it has "realtime"
// pressure to process.
//
// If this information does not influence your rendering code, then don't
// implement this extension.
typedef struct clap_plugin_render {
   // [main-thread]
   void (*set)(const clap_plugin *plugin, clap_plugin_render_mode mode);
} clap_plugin_render;

#ifdef __cplusplus
}
#endif