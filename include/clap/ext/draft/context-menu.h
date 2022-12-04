#pragma once

#include "../../plugin.h"

// This extension lets the host and plugin exchange menu items and let the plugin ask the host to
// show its context menu.

static CLAP_CONSTEXPR const char CLAP_EXT_CONTEXT_MENU[] = "clap.context-menu.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

// There can be different target kind for a context menu
enum {
   CLAP_CONTEXT_MENU_TARGET_KIND_GLOBAL,
   CLAP_CONTEXT_MENU_TARGET_KIND_PARAM,
};

// Describes the context menu target
typedef struct clap_context_menu_target
{
   uint32_t kind;
   clap_id id;
} clap_context_menu_target_t;

// Flags for the context menu accelerator
enum {
   CLAP_CONTEXT_MENU_ACCELERATOR_MODIFIER_SHIFT = (1 << 0),
   CLAP_CONTEXT_MENU_ACCELERATOR_MODIFIER_CONTROL = (1 << 1),
   CLAP_CONTEXT_MENU_ACCELERATOR_MODIFIER_ALT = (1 << 2),

   // Windows key or Command key
   CLAP_CONTEXT_MENU_ACCELERATOR_MODIFIER_META1 = (1 << 3),
};

// Describes a single key strike with its modifiers
typedef struct clap_context_menu_accelerator_key {
   uint32_t flags;
   uint32_t key; // ASCII key
} clap_context_menu_accelerator_key_t;

// Describes an accelerator which can consist of a key strike combo
typedef struct clap_context_menu_accelerator {
   uint32_t combo_size;
   clap_context_menu_accelerator_key_t keys[4];
} clap_context_menu_accelerator_t;

// Context menu builder
typedef struct clap_context_menu_builder {
   void *ctx;

   // accelerator may be null
   bool(CLAP_ABI *add_entry)(const struct clap_context_menu_builder *builder,
                             const char                             *label,
                             const clap_context_menu_accelerator_t  *accelerator,
                             bool                                    is_enabled,
                             clap_id                                 action_id);

   // accelerator may be null
   bool(CLAP_ABI *add_checkbox)(const struct clap_context_menu_builder *builder,
                                const char                             *label,
                                const clap_context_menu_accelerator_t  *accelerator,
                                bool                                    is_enabled,
                                bool                                    is_checked,
                                clap_id                                 action_id);

   bool(CLAP_ABI *add_separator)(const struct clap_context_menu_builder *builder);

   // Each begin_submenu() must have a corresponding end_submenu()
   bool(CLAP_ABI *begin_submenu)(const struct clap_context_menu_builder *builder,
                                 const char                             *label);

   bool(CLAP_ABI *end_submenu)(const struct clap_context_menu_builder *builder);
} clap_context_menu_builder_t;

typedef struct clap_plugin_context_menu {
   // If target is null, assume global context
   // [main-thread]
   bool(CLAP_ABI *populate)(const clap_plugin_t               *plugin,
                            const clap_context_menu_target_t  *target,
                            const clap_context_menu_builder_t *builder);

   // If target is null, assume global context
   // [main-thread]
   bool(CLAP_ABI *perform)(const clap_plugin_t              *plugin,
                           const clap_context_menu_target_t *target,
                           clap_id                           action_id);
} clap_plugin_context_menu_t;

typedef struct clap_host_context_menu {
   // If target is null, assume global context
   // [main-thread]
   bool(CLAP_ABI *populate)(const clap_host_t                 *host,
                            const clap_context_menu_target_t  *target,
                            const clap_context_menu_builder_t *builder);

   // If target is null, assume global context
   // [main-thread]
   bool(CLAP_ABI *perform)(const clap_host_t                *host,
                           const clap_context_menu_target_t *target,
                           clap_id action_id);

   // [main-thread]
   bool(CLAP_ABI *can_popup)(const clap_host_t *host);

   // Shows the host popup menu for a given parameter.
   // If the plugin is using embedded GUI, then x and y are relative to the plugin's window,
   // otherwise they're absolute coordinate, and screen index might be set accordingly.
   // If target is null, assume global context
   // [main-thread]
   bool(CLAP_ABI *popup)(const clap_host_t                *host,
                         const clap_context_menu_target_t *target,
                         int32_t                           screen_index,
                         int32_t                           x,
                         int32_t                           y);
} clap_host_context_menu_t;

#ifdef __cplusplus
}
#endif
