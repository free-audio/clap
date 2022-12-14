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
   // Sets or clears a mapping indication.
   // If the host doesn't use mapping color, then mapping_color should be null.
   // If is_mapped is false, then the mapping color is ignored.
   //
   // Parameter mapping indications should not be saved in the plugin context, and are off by default.
   // [main-thread]
   void(CLAP_ABI *set)(const clap_plugin_t *plugin,
                       clap_id              param_id,
                       bool                 is_mapped,
                       const clap_color_t  *mapping_color);
} clap_plugin_param_indication_t;

#ifdef __cplusplus
}
#endif
