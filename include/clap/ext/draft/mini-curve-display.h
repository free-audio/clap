#pragma once

#include "../../plugin.h"

// This extension allows a host to render a small curve provided by the plugin.
// A useful application is to render an EQ frequency response in the DAW mixer view.

static CLAP_CONSTEXPR const char CLAP_EXT_MINI_CURVE_DISPLAY[] = "clap.mini-curve-display/2";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_mini_curve_display_curve_kind {
   // The mini curve is intended to draw the total gain response of the plugin.
   // In this case the y values are in dB and the x values are in hz (logarithmic).
   // This would be useful in for example an equalizer.
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_GAIN_RESPONSE = 1,

   // The mini curve is intended to draw the total phase response of the plugin.
   // In this case the y values are in radians and the x values are in hz (logarithmic).
   // This would be useful in for example an equalizer.
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_PHASE_RESPONSE = 2,

   // The mini curve is intended to draw the transfer curve of the plugin.
   // In this case the both x and y values are in dB.
   // This would be useful in for example a compressor or distortion plugin.
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_TRANSFER_CURVE = 3,

   // This mini curve is intended to draw gain reduction over time. In this case
   // x refers to the window in seconds and y refers to level in dB, x_min is
   // always 0, and x_max would be the duration of the window.
   // This would be useful in for example a compressor or limiter.
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_GAIN_REDUCTION = 4,

   // This curve is intended as a generic time series plot. In this case
   // x refers to the window in seconds. x_min is always 0, and x_max would be the duration of the
   // window.
   // Y is not specified and up to the plugin.
   CLAP_MINI_CURVE_DISPLAY_CURVE_KIND_TIME_SERIES = 5,

   // Note: more entries could be added here in the future
};

typedef struct clap_mini_display_curve_hints {

   // Range for the x axis.
   double x_min;
   double x_max;

   // Range for the y axis.
   double y_min;
   double y_max;

} clap_mini_display_curve_hints_t;


typedef struct clap_plugin_mini_curve_display {
   // Renders the curve into the data buffer.
   // The value 0 will be at the bottom of the curve and UINT16_MAX will be at the top.
   // The value at index 0 will be the leftmost and the value at index data_size -1 will be the
   // rightmost.
   // [main-thread]
   bool(CLAP_ABI *render)(const clap_plugin_t *plugin, uint16_t *data, uint32_t data_size);

   // Tells the plugin if the curve is currently observed or not.
   // When it isn't observed render() can't be called.
   //
   // When is_obseverd becomes true, the curve content and axis name are implicitely invalidated. So
   // the plugin don't need to call host->changed.
   //
   // [main-thread]
   void(CLAP_ABI *set_observed)(const clap_plugin_t *plugin, bool is_observed);

   // Retrives the axis name.
   // x_name and y_name must not to be null.
   // Returns true on success, if the name capacity was sufficient.
   // [main-thread]
   bool(CLAP_ABI *get_axis_name)(const clap_plugin_t *plugin,
                                 char                *x_name,
                                 char                *y_name,
                                 uint32_t             name_capacity);
} clap_plugin_mini_curve_display_t;

enum clap_mini_curve_display_change_flags {
   // Informs the host that the curve content changed.
   // Can only be called if the curve is observed and is static.
   CLAP_MINI_CURVE_DISPLAY_CURVE_CHANGED = 1 << 0,

   // Informs the host that the curve axis name changed.
   // Can only be called if the curve is observed.
   CLAP_MINI_CURVE_DISPLAY_AXIS_NAME_CHANGED = 1 << 1,
};

typedef struct clap_host_mini_curve_display {
   // Fills in the given clap_mini_display_curve_hints_t structure and returns
   // true if succesful. If not, return false.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_host_t               *host,
      uint32_t                         kind,
      clap_mini_display_curve_hints_t *hints);

   // Mark the curve as being static or dynamic.
   // The curve is initially considered as static, though the plugin should explicitely
   // initialize this state.
   //
   // When static, the curve changes will be notified by calling host->changed().
   // When dynamic, the curve is constantly changing and the host is expected to
   // periodically re-render.
   //
   // [main-thread]
   void(CLAP_ABI *set_dynamic)(const clap_host_t *host, bool is_dynamic);

   // See clap_mini_curve_display_change_flags
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host, uint32_t flags);
} clap_host_mini_curve_display_t;

#ifdef __cplusplus
}
#endif
