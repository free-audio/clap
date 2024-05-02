#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_PRESET_LOAD[] = "clap.preset-load/2";

// The latest draft is 100% compatible.
// This compat ID may be removed in 2026.
static CLAP_CONSTEXPR const char CLAP_EXT_PRESET_LOAD_COMPAT[] = "clap.preset-load.draft/2";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_preset_load {
   // Loads a preset in the plugin native preset file format from a location.
   // The preset discovery provider defines the location and load_key to be passed to this function.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *from_location)(const clap_plugin_t *plugin,
                                 uint32_t             location_kind,
                                 const char          *location,
                                 const char          *load_key);
} clap_plugin_preset_load_t;

typedef struct clap_host_preset_load {
   // Called if clap_plugin_preset_load.load() failed.
   // os_error: the operating system error, if applicable. If not applicable set it to a non-error
   // value, eg: 0 on unix and Windows.
   //
   // [main-thread]
   void(CLAP_ABI *on_error)(const clap_host_t *host,
                            uint32_t           location_kind,
                            const char        *location,
                            const char        *load_key,
                            int32_t            os_error,
                            const char        *msg);

   // Informs the host that the following preset has been loaded.
   // This contributes to keep in sync the host preset browser and plugin preset browser.
   // If the preset was loaded from a container file, then the load_key must be set, otherwise it
   // must be null.
   //
   // [main-thread]
   void(CLAP_ABI *loaded)(const clap_host_t *host,
                          uint32_t           location_kind,
                          const char        *location,
                          const char        *load_key);
} clap_host_preset_load_t;

#ifdef __cplusplus
}
#endif
