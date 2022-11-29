#pragma once

#include "../../plugin.h"

// This extension lets the host and plugin exchange menu items and let the plugin ask the host to
// show its context menu.

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_INDICATION[] = "clap.context-menu.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_context_menu_builder {
   void *ctx;

   bool(CLAP_ABI *add_entry)(const struct clap_context_menu_builder *builder,
                             const char                             *label,
                             bool                                    is_enabled,
                             clap_id                                 action_id);
   bool(CLAP_ABI *add_checkbox)(const struct clap_context_menu_builder *builder,
                                const char                             *label,
                                bool                                    is_enabled,
                                bool                                    is_checked,
                                clap_id                                 action_id);
   bool(CLAP_ABI *add_separator)(const struct clap_context_menu_builder *builder);
   bool(CLAP_ABI *begin_submenu)(const struct clap_context_menu_builder *builder,
                                 const char                             *label);
   bool(CLAP_ABI *end_submenu)(const struct clap_context_menu_builder *builder);
} clap_context_menu_builder_t;

typedef struct clap_plugin_context_menu {
   // [main-thread]
   bool(CLAP_ABI *populate)(const clap_plugin_t               *plugin,
                            clap_id                            param_id,
                            const clap_context_menu_builder_t *builder);

   // [main-thread]
   bool(CLAP_ABI *perform)(const clap_plugin_t *plugin, clap_id param_id, clap_id action_id);
} clap_plugin_context_menu_t;

typedef struct clap_host_context_menu {
   // [main-thread]
   bool(CLAP_ABI *populate)(const clap_host_t                 *host,
                            clap_id                            param_id,
                            const clap_context_menu_builder_t *builder);

   // [main-thread]
   bool(CLAP_ABI *perform)(const clap_host_t *host, clap_id param_id, clap_id action_id);

   // [main-thread]
   bool(CLAP_ABI *can_popup)(const clap_host_t *host);

   // Shows the host popup menu for a given parameter.
   // If the plugin is using embedded GUI, then x and y are relative to the plugin's window,
   // otherwise they're absolute coordinate, and screen index might be set accordingly.
   // [main-thread]
   bool(CLAP_ABI *popup)(const clap_host_t *host, clap_id param_id, int32_t screen_index, int32_t x, int32_t y);
} clap_host_context_menu_t;

#ifdef __cplusplus
}
#endif
