#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_ORIGIN[] = "clap.param-origin/1";

#ifdef __cplusplus
extern "C" {
#endif

/// This extension provides an optional value per parameter that lets the host draw a visual
/// indication for the parameter's origin.
///
/// examples:
/// - lowpass filter cutoff parameter with an origin equal to param_info.min_value
///       [-------------->        ]
///      60Hz                   20kHz
///      min=origin              max
/// - highpass filter cutoff parameter with an origin equal to param_info.max_value
///       [        <--------------]
///      60Hz                   20kHz
///      min                     max=origin
/// - (bipolar) parameter with a range from -1.0 to +1.0 with an origin of 0.0
///       [     <------|          ]
///     -1.0          0.0       +1.0
///      min         origin      max
/// - crossfade parameter without an origin
///       [              o        ]
///       A                       B
///      min                     max

typedef struct clap_plugin_param_origin {
   // Get the origin value for a parameter.
   // Returns false if the parameter has no origin, true otherwise.
   // The host must not call this for params with CLAP_PARAM_IS_ENUM flag set.
   //
   // out_value constraints:
   // - has to be in the range from param_info.min_value to param_info.max_value
   // - has to be an integer value if CLAP_PARAM_IS_STEPPED flag is set
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t *plugin, clap_id param_id, double *out_value);
} clap_plugin_param_origin_t;

typedef struct clap_host_param_origin {
   // Informs the host that param origins have changed.
   //
   // Note: If the plugin calls params.rescan with CLAP_PARAM_RESCAN_ALL, all previously scanned
   // parameter origins must be considered invalid. It is thus not necessary for the plugin to call
   // param_origin.changed in this case.
   //
   // Note: This is useful if a parameter origin changes on-the-fly. For example a plugin might want
   // to change the origin of a filter cutoff frequency parameter when the corresponding filter type
   // (LP/BP/HP) has changed.
   //
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_param_origin_t;

#ifdef __cplusplus
}
#endif
