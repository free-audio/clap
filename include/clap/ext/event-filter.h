#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_EVENT_FILTER[] = "clap.event-filter";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

// This extension lets the host know which event types the plugin is interested
// in.
// The host will cache the set of accepted events before activating the plugin.
// The set of accepted events can't change while the plugin is active.
//
// If this extension is not provided by the plugin, then all events are accepted.
//
// If CLAP_EVENT_TRANSPORT is not accepted, then clap_process.transport may be null.
typedef struct clap_plugin_event_filter {
   // Returns true if the plugin is interested in the given event type.
   // [main-thread]
   bool (*accepts)(const clap_plugin_t *plugin, uint16_t space_id, uint16_t event_type);
} clap_plugin_event_filter_t;


typedef struct clap_host_event_filter {
   // Informs the host that the set of accepted event type changed.
   // This requires the plugin to be deactivated.
   // [main-thread]
   void (*changed)(const clap_host_t *host);
} clap_host_event_filter_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif