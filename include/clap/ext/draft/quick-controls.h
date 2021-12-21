#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_QUICK_CONTROLS[] = "clap.quick-controls.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

enum { CLAP_QUICK_CONTROLS_COUNT = 8 };

typedef struct clap_quick_controls_page {
   alignas(4) clap_id id;
   alignas(1) char name[CLAP_NAME_SIZE];
   alignas(1) char keywords[CLAP_KEYWORDS_SIZE];
   alignas(4) clap_id param_ids[CLAP_QUICK_CONTROLS_COUNT];
} clap_quick_controls_page_t;

typedef struct clap_plugin_quick_controls {
   // [main-thread]
   uint32_t (*count)(const clap_plugin_t *plugin);

   // [main-thread]
   bool (*get)(const clap_plugin_t *plugin, uint32_t page_index, clap_quick_controls_page_t *page);

   // [main-thread]
   void (*select)(const clap_plugin_t *plugin, clap_id page_id);

   // [main-thread]
   clap_id (*get_selected)(const clap_plugin_t *plugin);
} clap_plugin_quick_controls_t;

enum {
   CLAP_QUICK_CONTROLS_PAGES_CHANGED = 1 << 0,
   CLAP_QUICK_CONTROLS_SELECTED_PAGE_CHANGED = 1 << 1,
};
typedef uint32_t clap_quick_controls_changed_flags;

typedef struct clap_host_quick_controls {
   // Informs the host that the quick controls have changed.
   // [main-thread]
   void (*changed)(const clap_host_t *host, clap_quick_controls_changed_flags flags);
} clap_host_quick_controls_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif