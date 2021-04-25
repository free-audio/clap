#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../clap.h"

#define CLAP_EXT_IDLE "clap/idle"

typedef struct clap_plugin_idle {
   // IDLE time that can be used by the plugin on the main thread
   // [main-thread]
   void (*idle)(clap_plugin *plugin);
} clap_plugin_idle;

#ifdef __cplusplus
}
#endif