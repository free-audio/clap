#pragma once

#include "../../plugin.h"

// This extension lets the plugin report the current gain reduction to the host.

static CLAP_CONSTEXPR const char CLAP_EXT_GAIN_REDUCTION[] = "clap.gain-reduction/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gain_reduction {
  // Returns the current gain reduction in dB. the value is intended
  // for informational display, for example in a host meter or tooltip.
  //
  // The function should be called on the audio thread to ensure consistency
  // with processing time, although the host will likely use the value on
  // the main thread.
  //
  // The returned value is in DB. Zero means the plugin is applying no gain
  // reduction, or is not processing. A negative value means the plugin is
  // applying gain reduction. A positive value means the plugin is adding
  // gain. A single value is returned for all audio channels.
  //
  // [audio-thread]
  double(CLAP_ABI *get)(const clap_plugin_t *plugin);
} clap_plugin_gain_reduction_t;

#ifdef __cplusplus
}
#endif
