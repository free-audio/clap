#ifndef CLAP_EXT_GUI_H
#define CLAP_EXT_GUI_H

#include "../clap.h"

#define CLAP_EXT_GUI "clap/gui"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_gui {
   // Get the size of the plugin UI.
   // [main-thread]
   void (*get_size)(clap_plugin *plugin, int32_t *width, int32_t *height);

   // Sets the GUI scaling factor.
   // [main-thread]
   void (*set_scale)(clap_plugin *plugin, double scale);

   void (*show)(clap_plugin *plugin);
   void (*hide)(clap_plugin *plugin);

   // [main-thread]
   void (*close)(clap_plugin *plugin);
} clap_plugin_gui;

typedef struct clap_host_gui {
   /* Request the host to resize the client area to width, height.
    * Return true on success, false otherwise.
    * [thread-safe] */
   bool (*resize)(clap_host *host, int32_t width, int32_t height);
} clap_host_gui;

#ifdef __cplusplus
}
#endif

#endif /* !CLAP_EXT_GUI_H */
