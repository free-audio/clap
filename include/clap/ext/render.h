#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_RENDER[] = "clap.render";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // Default setting, for "realtime" processing
   CLAP_RENDER_REALTIME = 0,

   // For processing without realtime pressure
   // The plugin may use more expensive algorithms for higher sound quality.
   CLAP_RENDER_OFFLINE = 1,
};
typedef int32_t clap_plugin_render_mode;

// The render extension is used to let the plugin know if it has "realtime"
// pressure to process.
//
// If this information does not influence your rendering code, then don't
// implement this extension.
typedef struct clap_plugin_render {
   // Returns true if the plugin has a hard requirement to process in real-time.
   // This is especially useful for plugin acting as a proxy to an hardware device.
   // [main-thread]
   bool(CLAP_ABI *has_hard_realtime_requirement)(const clap_plugin_t *plugin);

   // Returns true if the rendering mode could be applied.
   // [main-thread]
   bool(CLAP_ABI *set)(const clap_plugin_t *plugin, clap_plugin_render_mode mode);
} clap_plugin_render_t;

#ifdef __cplusplus
}
#endif
