#ifndef CLAP_EXT_GUI_H
#define CLAP_EXT_GUI_H

#include "../clap.h"

#define CLAP_EXT_GUI "clap/gui"

#ifdef __cplusplus
extern "C" {
#endif

struct clap_plugin_gui {
   // Get the size of the plugin UI.
   // [main-thread]
   void (*get_size)(struct clap_plugin *plugin,
                    int32_t *           width,
                    int32_t *           height);

   // [main-thread]
   void (*close)(struct clap_plugin *plugin);
};

struct clap_host_gui {
   /* Request the host to resize the client area to width, height.
    * Return true on success, false otherwise.
    * [thread-safe] */
   bool (*resize)(struct clap_host *host, int32_t width, int32_t height);
};

#ifdef __cplusplus
}
#endif

#endif /* !CLAP_EXT_GUI_H */
