#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_EVENT_REGISTRY[] = "clap.event-registry";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host_event_registry {
   // Queries an event space id.
   // The space id 0 is reserved for CLAP's core events. See CLAP_CORE_EVENT_SPACE.
   //
   // Return false and sets *space_id to UINT16_MAX if the space name is unknown to the host.
   // [main-thread]
   bool(CLAP_ABI *query)(const clap_host_t *host, const char *space_name, uint16_t *space_id);
} clap_host_event_registry_t;

#ifdef __cplusplus
}
#endif
