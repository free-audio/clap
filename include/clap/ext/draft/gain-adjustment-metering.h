#pragma once

#include "../../plugin.h"

// This extension lets the plugin report the current gain adjustment
// (typically, gain reduction) to the host.

static CLAP_CONSTEXPR const char CLAP_EXT_GAIN_ADJUSTMENT_METERING[] = "clap.gain-adjustment-metering/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gain_adjustment_metering {
  // Returns the current gain adjustment in dB. The value is intended
  // for informational display, for example in a host meter or tooltip.
  // The returned value represents the gain adjustment that the plugin
  // applied to the last sample in the most recently processed block.
  //
  // The returned value is in dB. Zero means the plugin is applying no gain
  // reduction, or is not processing. A negative value means the plugin is
  // applying gain reduction, as with a compressor or limiter. A positive
  // value means the plugin is adding gain, as with an expander. The value
  // represents the dynamic gain reduction or expansion applied by the
  // plugin, before any make-up gain or other adjustment. A single value is
  // returned for all audio channels.
  //
  // [audio-thread]
  double(CLAP_ABI *get)(const clap_plugin_t *plugin);
} clap_plugin_gain_adjustment_metering_t;

#ifdef __cplusplus
}
#endif
