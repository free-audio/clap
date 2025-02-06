#pragma once

#include "../../plugin.h"

// This extension allows a host to render a small curve provided by the plugin.
// A useful application is to render an EQ frequency response in the DAW mixer view.

static CLAP_CONSTEXPR const char CLAP_EXT_MINI_CURVE_DISPLAY[] = "clap.mini-curve-display/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_mini_curve_display {
   // Renders the curve into the data buffer.
   // The value 0 will be at the bottom of the curve and UINT16_MAX will be at the top.
   // The value at index 0 will be the leftmost and the value at index data_size -1 will be the
   // rightmost.
   // [main-thread]
   bool(CLAP_ABI *render)(clap_plugin_t *plugin, uint16_t *data, uint32_t data_size);

   // Tells the plugin if the curve is currently observed or not.
   // When it isn't observed render() can't be called.
   // [main-thread]
   void(CLAP_ABI *set_observed)(clap_plugin_t *plugin, bool is_observed);
} clap_plugin_mini_curve_display_t;

typedef struct clap_host_mini_curve_display {
   // Mark the curve as being static or dynamic.
   // The curve is initially considered as static, though the plugin should explicitely
   // initialize this state.
   //
   // When static, the curve changes will be notified by calling host->changed().
   // When dynamic, the curve is constantly changing and the host is expected to
   // periodically re-render.
   //
   // [main-thread]
   void(CLAP_ABI *set_dynamic)(clap_host_t *host, bool is_dynamic);

   // Informs the host that the curve content changed.
   // Can only be called if the curve is observed and is static.
   // [main-thread]
   void(CLAP_ABI *changed)(clap_host_t *host);
} clap_host_mini_curve_display_t;

#ifdef __cplusplus
}
#endif
