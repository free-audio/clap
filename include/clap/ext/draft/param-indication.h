#pragma once

#include "../params.h"
#include "../../color.h"

// This extension lets the host tell the plugin to display a little color based indication on the parameter.
// This can be used to indicate:
// - a physical controller is mapped to a parameter
// - the parameter is current playing an automation
// - the parameter is overriding the automation
// - etc...
//
// The color semantic depends upon the host here and the goal is to have a consistent experience across all plugins.

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_INDICATION[] = "clap.param-indication.draft/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_param_indication {
   // Sets or clears an indication.
   // If the host doesn't use indicaton color, then indication_color should be null.
   // If has_indication is false, then the indication_color is ignored.
   //
   // Parameter indications should not be saved in the plugin context, and are off by default.
   // [main-thread]
   void(CLAP_ABI *set)(const clap_plugin_t *plugin,
                       clap_id              param_id,
                       bool                 has_indication,
                       const clap_color_t  *indication_color);
} clap_plugin_param_indication_t;

#ifdef __cplusplus
}
#endif
