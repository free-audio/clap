#pragma once

#include "../../plugin.h"

// This extension can be optionally provided by a host as an addition to clap.mini-curve-display
// in order to give the plugin more knowledge about how the mini curve display will be
// displayed.

static CLAP_CONSTEXPR const char CLAP_EXT_MINI_CURVE_DISPLAY_HINTS[] =
   "clap.mini-curve-display-hints/1";

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
};

typedef struct clap_mini_display_curve_hints {

   // Range for the x axis.
   double x_min;
   double x_max;

   // Range for the y axis.
   double y_min;
   double y_max;

} clap_mini_display_curve_hints_t;

typedef struct clap_host_mini_curve_display_hints {
   // Fills in the given clap_mini_display_curve_hints_t structure and returns
   // true if succesful. If not, return false.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_host_t               *host,
                       uint32_t                         kind,
                       clap_mini_display_curve_hints_t *hints);
} clap_host_mini_curve_display_hints_t;

#ifdef __cplusplus
}
#endif

