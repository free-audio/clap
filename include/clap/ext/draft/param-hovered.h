#pragma once

#include "../../plugin.h"

// This extension lets a plugin report to the host the parameter's clap_id of its last hovered UI
// control and whether it is currently hovered by a pointer device.

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_HOVERED[] = "clap.param-hovered/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host_param_hovered {
   // Plugin informs the host about a new hover state.
   //
   // Should be called whenever the hovered UI control's parameter ID changes or when it changes
   // from hovered to not being hovered.
   //
   // Only one parameter can be hovered at a time. Use CLAP_INVALID_ID as param_id if no parameter
   // is hovered.
   //
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host, clap_id param_id);
} clap_host_param_hovered_t;

#ifdef __cplusplus
}
#endif
