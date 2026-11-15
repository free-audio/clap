#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

// This extension let the plugin provide a structured way of mapping parameters to an hardware
// controller.
//
// This is done by providing a set of remote control pages organized by section.
// A page contains up to 8 controls, which references parameters using param_id.
//
// |`- [section:main]
// |    `- [name:main] performance controls
// |`- [section:osc]
// |   |`- [name:osc1] osc1 page
// |   |`- [name:osc2] osc2 page
// |   |`- [name:osc-sync] osc sync page
// |    `- [name:osc-noise] osc noise page
// |`- [section:filter]
// |   |`- [name:flt1] filter 1 page
// |    `- [name:flt2] filter 2 page
// |`- [section:env]
// |   |`- [name:env1] env1 page
// |    `- [name:env2] env2 page
// |`- [section:lfo]
// |   |`- [name:lfo1] env1 page
// |    `- [name:lfo2] env2 page
//  `- etc...
//
// One possible workflow is to have a set of buttons, which correspond to a section.
// Pressing that button once gets you to the first page of the section.
// Press it again to cycle through the section's pages.

static CLAP_CONSTEXPR const char CLAP_EXT_REMOTE_CONTROLS[] = "clap.remote-controls/2";

// The latest draft is 100% compatible
// This compat ID may be removed in 2026.
static CLAP_CONSTEXPR const char CLAP_EXT_REMOTE_CONTROLS_COMPAT[] = "clap.remote-controls.draft/2";

#ifdef __cplusplus
extern "C" {
#endif

enum { CLAP_REMOTE_CONTROLS_COUNT = 8 };

typedef struct clap_remote_controls_page {
   char    section_name[CLAP_NAME_SIZE];
   clap_id page_id;
   char    page_name[CLAP_NAME_SIZE];
   clap_id param_ids[CLAP_REMOTE_CONTROLS_COUNT];

   // This is used to separate device pages versus preset pages.
   // If true, then this page is specific to this preset.
   bool is_for_preset;
} clap_remote_controls_page_t;

typedef struct clap_plugin_remote_controls {
   // Returns the number of pages.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Get a page by index.
   // Returns true on success and stores the result into page.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t         *plugin,
                       uint32_t                     page_index,
                       clap_remote_controls_page_t *page);
} clap_plugin_remote_controls_t;

typedef struct clap_host_remote_controls {
   // Informs the host that the remote controls have changed.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);

   // Suggest a page to the host because it corresponds to what the user is currently editing in the
   // plugin's GUI.
   // [main-thread]
   void(CLAP_ABI *suggest_page)(const clap_host_t *host, clap_id page_id);
} clap_host_remote_controls_t;

#ifdef __cplusplus
}
#endif
