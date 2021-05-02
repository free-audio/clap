#pragma once

#include "../../clap.h"
#include "../audio-ports.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_REMOTE_CONTROLS "clap/draft/remote-controls"

typedef struct clap_remote_controls_page {
   int32_t id;
   char    name[CLAP_NAME_SIZE];
   int32_t param_id[8];
} clap_remote_controls_page;

typedef struct clap_plugin_remote_controls {
   int32_t (*count)(clap_plugin *plugin);

   // [main-thread]
   bool (*get_page)(clap_plugin *plugin, int32_t page_index, clap_remote_controls_page *page);

   // [main-thread]
   void (*select_page)(clap_plugin *plugin, int32_t page_id);
} clap_host_plugin_info;

typedef struct clap_host_remote_controls {
   // Informs the host that the remote controls page has changed.
   // [main-thread]
   void (*changed)(clap_host *host, clap_plugin *plugin);
} clap_host_remote_controls;

#ifdef __cplusplus
}
#endif