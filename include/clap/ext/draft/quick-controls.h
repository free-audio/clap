#pragma once

#include "../../clap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_QUICK_CONTROLS "clap/draft/quick-controls"

typedef struct clap_quick_controls_page {
   clap_id id;
   char    name[CLAP_NAME_SIZE];
   char    keywords[CLAP_KEYWORDS_SIZE];
   clap_id param_id[8];
} clap_quick_controls_page;

typedef struct clap_plugin_quick_controls {
   int32_t (*page_count)(clap_plugin *plugin);

   // [main-thread]
   bool (*get_page)(clap_plugin *plugin, int32_t page_index, clap_quick_controls_page *page);

   // [main-thread]
   void (*select_page)(clap_plugin *plugin, clap_id page_id);

   // [main-thread]
   clap_id (*get_selected_page)(clap_plugin *plugin);
} clap_host_plugin_info;

typedef struct clap_host_quick_controls {
   // Informs the host that the quick controls pages has changed.
   // [main-thread]
   void (*pages_changed)(clap_host *host);

   // Informs the host that the user did change the selected page, from the plugin.
   // [main-thread]
   void (*selected_page_changed)(clap_host *host, clap_id page_id);
} clap_host_quick_controls;

#ifdef __cplusplus
}
#endif