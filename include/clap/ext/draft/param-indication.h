#pragma once

#include "../params.h"
#include "../../color.h"

// This extension lets the host tell the plugin to display a little color based indication on the
// parameter. This can be used to indicate:
// - a physical controller is mapped to a parameter
// - the parameter is current playing an automation
// - the parameter is overriding the automation
// - etc...
//
// The color semantic depends upon the host here and the goal is to have a consistent experience
// across all plugins.

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_INDICATION[] = "clap.param-indication.draft/2";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_PARAM_INDICATION_AUTOMATION_NONE,
   CLAP_PARAM_INDICATION_AUTOMATION_PLAYBACK,
   CLAP_PARAM_INDICATION_AUTOMATION_RECORD,
   CLAP_PARAM_INDICATION_AUTOMATION_OVERRIDE,
};

typedef struct clap_plugin_param_indication {
   // Sets or clears a mapping indication.
   // If the host doesn't use an indicaton color, then color should be null.
   // If has_mapping is false, then the color is ignored.
   //
   // Parameter indications should not be saved in the plugin context, and are off by default.
   // [main-thread]
   void(CLAP_ABI *set_mapping)(const clap_plugin_t *plugin,
                               clap_id              param_id,
                               bool                 has_mapping,
                               const clap_color_t  *color);

   // Sets or clears an automation indication.
   // If the host doesn't use an indicaton color, then color should be null.
   // If automation_state is CLAP_PARAM_INDICATION_AUTOMATION_NONE, then the color is ignored.
   //
   // Parameter indications should not be saved in the plugin context, and are off by default.
   // [main-thread]
   void(CLAP_ABI *set_automation)(const clap_plugin_t *plugin,
                                  clap_id              param_id,
                                  uint32_t             automation_state,
                                  const clap_color_t  *color);
} clap_plugin_param_indication_t;

#ifdef __cplusplus
}
#endif
