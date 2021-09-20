#pragma once

#include "../../clap.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_QUICK_CONTROLS[] = "clap.quick-controls.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum { CLAP_QUICK_CONTROLS_COUNT = 8 };

typedef struct clap_quick_controls_page {
   clap_id id;
   char    name[CLAP_NAME_SIZE];
   char    keywords[CLAP_KEYWORDS_SIZE];
   clap_id param_ids[CLAP_QUICK_CONTROLS_COUNT];
} clap_quick_controls_page;

typedef struct clap_plugin_quick_controls {
   // [main-thread]
   uint32_t (*count)(const clap_plugin *plugin);

   // [main-thread]
   bool (*get)(const clap_plugin *plugin, uint32_t page_index, clap_quick_controls_page *page);

   // [main-thread]
   void (*select)(const clap_plugin *plugin, clap_id page_id);

   // [main-thread]
   clap_id (*get_selected)(const clap_plugin *plugin);
} clap_plugin_quick_controls;

enum {
   CLAP_QUICK_CONTROLS_PAGES_CHANGED = 1 << 0,
   CLAP_QUICK_CONTROLS_SELECTED_PAGE_CHANGED = 1 << 1,
};
typedef uint32_t clap_quick_controls_changed_flags;

typedef struct clap_host_quick_controls {
   // Informs the host that the quick controls have changed.
   // [main-thread]
   void (*changed)(const clap_host *host, clap_quick_controls_changed_flags flags);
} clap_host_quick_controls;

#ifdef __cplusplus
}
#endif