#pragma once

#include "../../plugin.h"

// This extension lets the plugin report the current gain reduction to the host.

static CLAP_CONSTEXPR const char CLAP_EXT_GAIN_REDUCTION[] = "clap.gain-reduction/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gain_reduction {
  // reports the current gain reduction in dB. the value is intended
  // for informational display only, for example in a host meter or tooltip.
   double(CLAP_ABI *current_gain_reduction_db)(const clap_plugin_t *plugin);
} clap_plugin_gain_reduction_t;

#ifdef __cplusplus
}
#endif
