#pragma once

#include "../clap.h"
#include "../stream.h"

#define CLAP_EXT_STATE "clap/state"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_state {
   /* Saves the plugin state into stream.
    * [main-thread] */
   bool (*save)(const clap_plugin *plugin, clap_ostream *stream);

   /* Loads the plugin state from stream.
    * [main-thread] */
   bool (*load)(const clap_plugin *plugin, clap_istream *stream);

   // [main-thread]
   bool (*is_dirty)(const clap_plugin *plugin);
} clap_plugin_state;

typedef struct clap_host_state {
   /* Tell the host that the plugin state has changed.
    * [thread-safe] */
   void (*mark_dirty)(const clap_host *host);
} clap_host_state;

#ifdef __cplusplus
}
#endif
