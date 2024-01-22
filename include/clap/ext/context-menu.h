#pragma once

#include "../plugin.h"

// This extension lets the host and plugin exchange menu items and let the plugin ask the host to
// show its context menu.

static CLAP_CONSTEXPR const char CLAP_EXT_CONTEXT_MENU[] = "clap.context-menu/1";

// The latest draft is 100% compatible.
// This compat ID may be removed in 2026.
static CLAP_CONSTEXPR const char CLAP_EXT_CONTEXT_MENU_COMPAT[] = "clap.context-menu.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

// There can be different target kind for a context menu
enum {
   CLAP_CONTEXT_MENU_TARGET_KIND_GLOBAL = 0,
   CLAP_CONTEXT_MENU_TARGET_KIND_PARAM = 1,
};

// Describes the context menu target
typedef struct clap_context_menu_target {
   uint32_t kind;
   clap_id  id;
} clap_context_menu_target_t;

enum {
   // Adds a clickable menu entry.
   // data: const clap_context_menu_item_entry_t*
   CLAP_CONTEXT_MENU_ITEM_ENTRY,

   // Adds a clickable menu entry which will feature both a checkmark and a label.
   // data: const clap_context_menu_item_check_entry_t*
   CLAP_CONTEXT_MENU_ITEM_CHECK_ENTRY,

   // Adds a separator line.
   // data: NULL
   CLAP_CONTEXT_MENU_ITEM_SEPARATOR,

   // Starts a sub menu with the given label.
   // data: const clap_context_menu_item_begin_submenu_t*
   CLAP_CONTEXT_MENU_ITEM_BEGIN_SUBMENU,

   // Ends the current sub menu.
   // data: NULL
   CLAP_CONTEXT_MENU_ITEM_END_SUBMENU,

   // Adds a title entry
   // data: const clap_context_menu_item_title_t *
   CLAP_CONTEXT_MENU_ITEM_TITLE,
};
typedef uint32_t clap_context_menu_item_kind_t;

typedef struct clap_context_menu_entry {
   // text to be displayed
   const char *label;

   // if false, then the menu entry is greyed out and not clickable
   bool        is_enabled;
   clap_id     action_id;
} clap_context_menu_entry_t;

typedef struct clap_context_menu_check_entry {
   // text to be displayed
   const char *label;

   // if false, then the menu entry is greyed out and not clickable
   bool        is_enabled;

   // if true, then the menu entry will be displayed as checked
   bool        is_checked;
   clap_id     action_id;
} clap_context_menu_check_entry_t;

typedef struct clap_context_menu_item_title {
   // text to be displayed
   const char *title;

   // if false, then the menu entry is greyed out
   bool        is_enabled;
} clap_context_menu_item_title_t;

typedef struct clap_context_menu_submenu {
   // text to be displayed
   const char *label;

   // if false, then the menu entry is greyed out and won't show submenu
   bool        is_enabled;
} clap_context_menu_submenu_t;

// Context menu builder.
// This object isn't thread-safe and must be used on the same thread as it was provided.
typedef struct clap_context_menu_builder {
   void *ctx;

   // Adds an entry to the menu.
   // item_data type is determined by item_kind.
   // Returns true on success.
   bool(CLAP_ABI *add_item)(const struct clap_context_menu_builder *builder,
                            clap_context_menu_item_kind_t           item_kind,
                            const void                             *item_data);

   // Returns true if the menu builder supports the given item kind
   bool(CLAP_ABI *supports)(const struct clap_context_menu_builder *builder,
                            clap_context_menu_item_kind_t           item_kind);
} clap_context_menu_builder_t;

typedef struct clap_plugin_context_menu {
   // Insert plugin's menu items into the menu builder.
   // If target is null, assume global context.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *populate)(const clap_plugin_t               *plugin,
                            const clap_context_menu_target_t  *target,
                            const clap_context_menu_builder_t *builder);

   // Performs the given action, which was previously provided to the host via populate().
   // If target is null, assume global context.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *perform)(const clap_plugin_t              *plugin,
                           const clap_context_menu_target_t *target,
                           clap_id                           action_id);
} clap_plugin_context_menu_t;

typedef struct clap_host_context_menu {
   // Insert host's menu items into the menu builder.
   // If target is null, assume global context.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *populate)(const clap_host_t                 *host,
                            const clap_context_menu_target_t  *target,
                            const clap_context_menu_builder_t *builder);

   // Performs the given action, which was previously provided to the plugin via populate().
   // If target is null, assume global context.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *perform)(const clap_host_t                *host,
                           const clap_context_menu_target_t *target,
                           clap_id                           action_id);

   // Returns true if the host can display a popup menu for the plugin.
   // This may depend upon the current windowing system used to display the plugin, so the
   // return value is invalidated after creating the plugin window.
   // [main-thread]
   bool(CLAP_ABI *can_popup)(const clap_host_t *host);

   // Shows the host popup menu for a given parameter.
   // If the plugin is using embedded GUI, then x and y are relative to the plugin's window,
   // otherwise they're absolute coordinate, and screen index might be set accordingly.
   // If target is null, assume global context.
   // Returns true on success.
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
