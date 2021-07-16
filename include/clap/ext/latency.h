#pragma once

#include "../clap.h"
#include <stdint.h>

static CLAP_CONSTEXPR const char CLAP_EXT_LATENCY[] = "clap.latency";

#ifdef __cplusplus
extern "C" {
#endif

// The audio ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_latency {
   // Returns the plugin latency.
   // [main-thread]
   uint32_t (*get)(const clap_plugin *plugin);
} clap_plugin_latency;

typedef struct clap_host_latency {
   // Tell the host that the latency changed.
   // The latency is only allowed to change if the plugin is deactivated.
   // If the plugin is activated, call host->request_restart()
   // [main-thread]
   void (*changed)(const clap_host *host);
} clap_host_latency;

#ifdef __cplusplus
}
#endif
