#pragma once

#include "../clap.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_LATENCY "clap/latency"

// The audio ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_latency {
   // Returns the plugin latency.
   // [main-thread]
   uint32_t (*get)(const clap_plugin *plugin);
} clap_plugin_latency;

typedef struct clap_host_latency {
   // Tell the host that the latency changed.
   // The new latency will be effective after deactivation of the plugin.
   // [main-thread]
   void (*changed)(const clap_host *host, uint32_t new_latency);
} clap_host_latency;

#ifdef __cplusplus
}
#endif
