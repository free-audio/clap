#pragma once

#include "../params.h"
#include "../../color.h"

// This extension lets the host tell the plugin that a parameter is currently
// mapped to a physical controller.
//
// It is common that those controllers works using a bank of 8 knobs, and to
// improve readability the host may assign a color to each physical knobs and
// indicate them on the GUI.

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_INDICATION[] = "clap.param-indication.draft/0";

typedef struct clap_plugin_param_indication {
   // Sets or clears a mapping indication.
   // [main-thread]
   void(CLAP_ABI *set_indication)(const clap_plugin_t *plugin,
                                  clap_id              param_id,
                                  bool                 is_mapped,
                                  clap_color_t         mapping_color);
} clap_plugin_param_indication_t;