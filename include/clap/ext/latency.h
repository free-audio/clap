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
   uint32_t (*get_latency)(clap_plugin *plugin);
} clap_plugin_audio_ports;

typedef struct clap_host_audio_ports {
   // Tell the host that the latency changed.
   // The new latency will be effective after deactivation of the plugin.
   // [main-thread]
   void (*changed)(clap_host *host, clap_plugin *plugin, uint32_t new_latency);
} clap_host_audio_ports;

#ifdef __cplusplus
}
#endif
