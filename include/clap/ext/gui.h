#pragma once

#include "../clap.h"

static CLAP_CONSTEXPR const char CLAP_EXT_GUI[] = "clap/gui";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gui {
   // Set the GUI scaling factor.
   // [main-thread]
   void (*set_scale)(const clap_plugin *plugin, double scale);

   // Get the size of the plugin UI, with the scaling applied.
   // [main-thread]
   void (*size)(const clap_plugin *plugin, int32_t *width, int32_t *height);

   // [main-thread]
   bool (*can_resize)(const clap_plugin *plugin);

   // If the plugin gui is resizable, then the plugin will return the closest size
   // to the given arguments.
   //
   // [main-thread]
   void (*round_size)(const clap_plugin *plugin, int32_t *width, int32_t *height);

   // [main-thread]
   void (*show)(const clap_plugin *plugin);

   // [main-thread]
   void (*hide)(const clap_plugin *plugin);

   // [main-thread]
   void (*close)(const clap_plugin *plugin);
} clap_plugin_gui;

typedef struct clap_host_gui {
   /* Request the host to resize the client area to width, height.
    * Return true on success, false otherwise.
    * [thread-safe] */
   bool (*resize)(const clap_host *host, int32_t width, int32_t height);
} clap_host_gui;

#ifdef __cplusplus
}
#endif