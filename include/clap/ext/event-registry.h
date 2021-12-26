#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_EVENT_REGISTRY[] = "clap.event-registry";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_host_event_registry {
   // Queries an event space id.
   //
   // The first 1024 space_ids are reserved.
   //
   // Return false and sets *space_id to UINT16_MAX if the space name is unknown to the host.
   // [main-thread]
   bool (*query)(const clap_host_t *host, const char *space_name, uint16_t *space_id);
} clap_host_event_registry_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
