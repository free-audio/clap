#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_LATENCY[] = "clap.latency";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_latency {
   // Returns the plugin latency in samples.
   // [main-thread & active]
   uint32_t(CLAP_ABI *get)(const clap_plugin_t *plugin);
} clap_plugin_latency_t;

typedef struct clap_host_latency {
   // Tell the host that the latency changed.
   // The latency is only allowed to change if the plugin is deactivated.
   // If the plugin is activated, call host->request_restart()
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_latency_t;

#ifdef __cplusplus
}
#endif
