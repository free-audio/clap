#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_EVENT_FILTER "clap/draft/event_filter"

// This extension lets the host know which event types the plugin is interested
// in.
typedef struct clap_plugin_event_filter {
   // Returns true if the plugin is interested in the given event type.
   // [audio-thread]
   bool (*accepts)(clap_plugin *plugin, clap_event_type event_type);
} clap_plugin_event_filter;


typedef struct clap_host_event_filter {
   void (*changed)(clap_host *host);
} clap_host_event_filter;

#ifdef __cplusplus
}
#endif