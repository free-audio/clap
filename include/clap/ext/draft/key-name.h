#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_KEY_NAME "clap/draft/key-name"

struct clap_key_name {
   char   name[CLAP_NAME_SIZE];
   int8_t key;
   int8_t channel; // -1 for every channels
};

struct clap_plugin_key_name {
   // Return the number of key names
   // [main-thread]
   int (*count)(struct clap_plugin *plugin);

   // Returns true on success and stores the result into key_name
   // [main-thread]
   bool (*get)(struct clap_plugin *  plugin,
               int                   index,
               struct clap_key_name *key_name);
};

struct clap_host_key_name {
   // Informs the host that the drum map has changed.
   // [main-thread]
   void (*changed)(struct clap_host *host, struct clap_plugin *plugin);
};

#ifdef __cplusplus
}
#endif