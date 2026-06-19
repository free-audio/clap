#pragma once

#include "../../plugin.h"

// This extension lets a plugin report to the host the parameter's clap_id of its last focused UI
// control and whether it is currently focused (usually hovered by pointer device).

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_FOCUSED[] = "clap.param-hovered/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host_param_focused {
   // Plugin informs the host about a new focus state.
   //
   // Should be called whenever the focused UI control's parameter ID changes or when it changes
   // from focused to not being focused.
   //
   // Only one parameter can be focused at a time. Use CLAP_INVALID_ID as param_id if no parameter
   // is focused.
   //
   // [main-thread]
   void(CLAP_ABI *update)(const clap_host_t *host, clap_id param_id);
} clap_host_param_focused_t;

#ifdef __cplusplus
}
#endif
