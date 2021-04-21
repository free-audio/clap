#pragma once

#include "../clap.h"
#include "stream.h"

#define CLAP_EXT_STATE "clap/state"

#ifdef __cplusplus
extern "C" {
#endif

struct clap_plugin_state {
   /* Saves the plugin state into stream.
    * [main-thread] */
   bool (*save)(struct clap_plugin *plugin, struct clap_ostream *stream);

   /* Loads the plugin state from stream.
    * [main-thread] */
   bool (*restore)(struct clap_plugin *plugin, struct clap_istream *stream);
};

struct clap_host_state {
   /* Tell the host that the plugin state has changed.
    * [thread-safe] */
   void (*set_dirty)(struct clap_host *host, struct clap_plugin *plugin);
};

#ifdef __cplusplus
}
#endif
