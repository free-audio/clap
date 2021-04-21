#pragma once

#include "../clap.h"

#define CLAP_EXT_RENDER "clap/render"

#ifdef __cplusplus
extern "C" {
#endif

enum clap_plugin_render_mode {
   /* Default setting, used to play "realtime". */
   CLAP_RENDER_REALTIME = 0,

   /* Render setting, used while rendering the song. */
   CLAP_RENDER_OFFLINE = 1,
};

// The render extension is used to let the plugin know if it has "realtime"
// pressure to process.
struct clap_plugin_render {
   // [main-thread]
   void (*set_render_mode)(struct clap_plugin *         plugin,
                           enum clap_plugin_render_mode mode);
};

#ifdef __cplusplus
}
#endif