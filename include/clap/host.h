#pragma once

#include "version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host {
   clap_version_t clap_version; // initialized to CLAP_VERSION

   void *host_data; // reserved pointer for the host

   // name and version are mandatory.
   const char *name;    // eg: "Bitwig Studio"
   const char *vendor;  // eg: "Bitwig GmbH"
   const char *url;     // eg: "https://bitwig.com"
   const char *version; // eg: "4.3", see plugin.h for advice on how to format the version

   // Query an extension.
   // The returned pointer is owned by the host.
   // It is forbidden to call it before plugin->init().
   // You can call it within plugin->init() call, and after.
   // [thread-safe]
   const void *(CLAP_ABI *get_extension)(const struct clap_host *host, const char *extension_id);

   // Request the host to deactivate and then reactivate the plugin.
   // The operation may be delayed by the host.
   // [thread-safe]
   void(CLAP_ABI *request_restart)(const struct clap_host *host);

   // Request the host to activate and start processing the plugin.
   // This is useful if you have external IO and need to wake up the plugin from "sleep".
   // [thread-safe]
   void(CLAP_ABI *request_process)(const struct clap_host *host);

   // Request the host to schedule a call to plugin->on_main_thread(plugin) on the main thread.
   // This callback should be called as soon as practicable, usually in the host application's next
   // available main thread time slice. Typically callbacks occur withink 33ms / 30hz.
   // Despite this guidance, plugins should not make assumptions about the exactness of timing for
   // a main thread callback, but hosts should endeavour to be prompt. For example, in high load situations
   // the environment may starve the gui/main thread in favor of audio processing, leading to substantially
   // longer latencies for the callback than the indicative times given here.
   // [thread-safe]
   void(CLAP_ABI *request_callback)(const struct clap_host *host);
} clap_host_t;

#ifdef __cplusplus
}
#endif
