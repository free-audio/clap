#pragma once

#include "../../plugin.h"

// This extension lets a plugin report to the host the parameter's clap_id of its last hovered UI
// control and whether it is currently hovered by a pointer device.

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_HOVERED[] = "clap.param-hovered/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_param_hovered {
   // Host informs the plugin about being observed regarding param-hovered.
   //
   // This extension is only meaningful while the plugin's UI is open. Each time after the plugin
   // opens its UI, the initial observation state is unobserved.
   //
   // After set_observed(..., true), the plugin must call one initial host->changed() as soon as
   // possible.
   // After set_observed(..., false), the plugin must stop calling host->changed().
   //
   // [main-thread]
   void(CLAP_ABI *set_observed)(const clap_plugin_t *plugin, bool is_observed);
} clap_plugin_param_hovered_t;

typedef struct clap_host_param_hovered {
   // Plugin informs the host about a new hover state.
   //
   // Should be called:
   // - once after observation has been enabled
   // - whenever (param_id, is_hovered) changes while observed
   //
   // If is_hovered is true, param_id must not be CLAP_INVALID_ID and should be set to the clap_id
   // of the parameter currently hovered in the UI.
   //
   // If is_hovered is false, param_id is either set to the last hovered UI control's parameter id
   // since the UI has been opened, or to CLAP_INVALID_ID in case no UI control has been hovered
   // yet.
   //
   // [main-thread & plugin-observed]
   void(CLAP_ABI *changed)(const clap_host_t *host, clap_id param_id, bool is_hovered);
} clap_host_param_hovered_t;

#ifdef __cplusplus
}
#endif
