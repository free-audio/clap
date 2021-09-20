#pragma once

#include "version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host {
   clap_version clap_version; // initialized to CLAP_VERSION

   void *host_data; // reserved pointer for the host

   // name and version are mandatory.
   const char *name;    // eg: "Bitwig Studio"
   const char *vendor;  // eg: "Bitwig GmbH"
   const char *url;     // eg: "https://bitwig.com"
   const char *version; // eg: "3.3.8"

   // Query an extension.
   // [thread-safe]
   const void *(*get_extension)(const struct clap_host *host, const char *extension_id);

   // Request the host to deactivate and then reactivate the plugin.
   // The operation may be delayed by the host.
   // [thread-safe]
   void (*request_restart)(const struct clap_host *host);

   // Request the host to activate and start processing the plugin.
   // This is useful if you have external IO and need to wake up the plugin from "sleep".
   // [thread-safe]
   void (*request_process)(const struct clap_host *host);

   // Request the host to schedule a call to plugin->on_main_thread(plugin) on the main thread.
   // [thread-safe]
   void (*request_callback)(const struct clap_host *host);
} clap_host;

#ifdef __cplusplus
}
#endif