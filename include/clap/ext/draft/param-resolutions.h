#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_RESOLUTIONS[] = "clap.param-resolutions/1";

#ifdef __cplusplus
extern "C" {
#endif

/// This extension allows a plugin to provides discrete value resolutions for parameters.
///
/// The host can use the parameter resolution for example for
/// - incrementing/decrementing the parameter value in discrete steps
///   (via mousewheel, up/down keys or relative hardware encoders, ...)
/// - drawing a grid for a generic parameter gui control
///
/// The plugin can provide additional resolution levels (fine/coarse)
/// which the host can use e.g. to provide more fine/coarse control over a parameter if the user
/// holds a key modifier while incrementing/decrementing its value.
///
/// The term "resolution" in this context describes the discrete number of segments in which the
/// value range from param_info.min_value to param_info.max_value is split into.
/// The host could, for example, calculate a discrete step delta for a given `resolution`
/// using `(param_info.max_value - param_info.min_value) / resolution`.
///
/// Note: For the following examples, the term "resulting display value" means the resulting text
/// after using the plugin's value-to-text method on the actual (double) parameter value.
///
/// Resolution examples:
/// - Simple parameter with a value range from param_info.min_value=0.0 to param_info.max_value=1.0
///   - Resulting values:
///     | resolution |          values          |
///     |============|==========================|
///     |          1 | 0.0                  1.0 |
///     |          2 | 0.0       0.5        1.0 |
///     |          4 | 0.0  0.25 0.5  0.75  1.0 |
///     |         10 | 0.0 0.1 0.2 ...  0.9 1.0 |
/// - Bipolar parameter with
///   - A value range from param_info.min_value=0.0 to param_info.max_value=1.0
///   - A display value range from -1.0 to 1.0 (after using value-to-text)
///   - resulting values:
///     | resolution |          values          |  resulting display values  |
///     |============|==========================|============================|
///     |          1 | 0.0                  1.0 | -1.0                   1.0 |
///     |          2 | 0.0       0.5        1.0 | -1.0        0.0        1.0 |
///     |          4 | 0.0  0.25 0.5  0.75  1.0 | -1.0  -0.5  0.5  0.75  1.0 |
///     |         20 | 0.0 0.1 0.2 ...  0.9 1.0 | -1.0 -0.9 -0.8 ... 0.9 1.0 |
///
/// Use case example:
/// - A 'Transpose' parameter with a display value range from -24st to +24st
///   - clap_param_resolutions:
///     - base_resolution: 48
///     - fine_resolution_factor: 100 (resulting fine resolution: 48 * 100 = 4800)
///     - coarse_resolution_divisor: 12 (resulting coarse resolution: 48 / 12 = 4)
///   - Resulting display values:
///     |   resolution  |        resulting display values (in st)     |
///     |===============|=============================================|
///     | (fine)   4800 | -24 -24.01 ... 0 ... +23.99 +24     (cents) |
///     | (base)     48 | -24 -23 ...    0 ... +23    +24 (semitones) |
///     | (coarse)    4 | -24     -12    0   +12      +24   (octaves) |

typedef struct clap_param_resolutions {
   // Base resolution of the parameter.
   //
   // Constraint: has to be >= 1
   uint16_t base_resolution;
   // Factor to multiply base_resolution with for a finer resolution
   // Resulting fine resolution: base_resolution * fine_resolution_factor
   // Set to 1 if there is no special fine resolution present
   //
   // Constraints:
   // - has to be >= 1
   // - for parameters with CLAP_PARAM_IS_STEPPED flag set, the resulting fine resolution must be
   // equal to `param_info.max_value - param_info.min_value`
   uint16_t fine_resolution_factor;
   // Divisor which to divide base_resolution by for a more coarse resolution
   // Resulting coarse resolution: base_resolution / fine_resolution_factor
   // Set to 1 if there is no special coarse resolution present
   //
   // Constraints:
   // - has to be >= 1
   // - base_resolution has to be divisible by coarse_resolution_divisor
   uint16_t coarse_resolution_divisor;
} clap_param_resolutions_t;

typedef struct clap_plugin_param_resolutions {
   // Get the resolutions for a parameter.
   // Returns false if there is no discrete resolution available for the parameter with the given
   // id.
   // The host must not call this for parameters with CLAP_PARAM_IS_ENUM flag set.
   //
   // Note: If this returns false for a parameter with CLAP_PARAM_IS_STEPPED flag set, the parameter
   // has only the implicit base_resolution of `param_info.max_value - param_info.min_value`.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t    *plugin,
                       clap_id                 param_id,
                       clap_param_resolutions *resolutions);

} clap_plugin_param_resolutions_t;

typedef struct clap_host_param_resolutions {
   // Informs the host that param resolutions have changed.
   //
   // Note: If the plugin calls params.rescan with CLAP_PARAM_RESCAN_ALL, all previously scanned
   // parameter resolutions must be considered invalid. It is thus not necessary for the plugin to
   // call param_resolution.changed in this case.
   //
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_param_resolutions_t;
#ifdef __cplusplus
}
#endif
