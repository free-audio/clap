#pragma once

#include "../clap.h"
#include "../stream.h"

static CLAP_CONSTEXPR const char CLAP_EXT_STATE[] = "clap.state";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_state {
   // Saves the plugin state into stream.
   // Returns true if the state was correctly saved.
   // [main-thread]
   bool (*save)(const clap_plugin *plugin, clap_ostream *stream);

   // Loads the plugin state from stream.
   // Returns true if the state was correctly restored.
   // [main-thread]
   bool (*load)(const clap_plugin *plugin, clap_istream *stream);
} clap_plugin_state;

typedef struct clap_host_state {
   // Tell the host that the plugin state has changed and should be saved again.
   // If a parameter value changes, then it is implicit that the state is dirty.
   // [main-thread]
   void (*mark_dirty)(const clap_host *host);
} clap_host_state;

#ifdef __cplusplus
}
#endif
