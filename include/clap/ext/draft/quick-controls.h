#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"

// This extensions provides a set of pages, where each page contains up to 8 controls.
// Those controls are param_id, and they are meant to be mapped onto a physical controller.
// We chose 8 because this what most controllers offer, and it is more or less a standard.

static CLAP_CONSTEXPR const char CLAP_EXT_QUICK_CONTROLS[] = "clap.quick-controls.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum { CLAP_QUICK_CONTROLS_COUNT = 8 };

typedef struct clap_quick_controls_page {
   clap_id id;
   char    name[CLAP_NAME_SIZE];
   clap_id param_ids[CLAP_QUICK_CONTROLS_COUNT];
} clap_quick_controls_page_t;

typedef struct clap_plugin_quick_controls {
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t        *plugin,
                       uint32_t                    page_index,
                       clap_quick_controls_page_t *page);
} clap_plugin_quick_controls_t;

typedef struct clap_host_quick_controls {
   // Informs the host that the quick controls have changed.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);

   // Suggest a page to the host because it correspond to what the user is currently editing in the
   // plugin's GUI.
   // [main-thread]
   void(CLAP_ABI *suggest_page)(const clap_host_t *host, clap_id page_id);
} clap_host_quick_controls_t;

#ifdef __cplusplus
}
#endif
