#pragma once

#include "../../plugin.h"

// This extension can be used to specify the cv channel type used by the plugin.
// Work in progress, suggestions are welcome

static CLAP_CONSTEXPR const char CLAP_EXT_CV[] = "clap.cv.draft/0";
static CLAP_CONSTEXPR const char CLAP_PORT_CV[] = "cv";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // TODO: standardize values?
   CLAP_CV_VALUE = 0,
   CLAP_CV_GATE = 1,
   CLAP_CV_PITCH = 2,
};

// TODO: maybe we want a channel_info instead, where we could have more details about the supported
// ranges?

typedef struct clap_plugin_cv {
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *get_channel_type)(const clap_plugin_t *plugin,
                                    bool                 is_input,
                                    uint32_t             port_index,
                                    uint32_t             channel_index,
                                    uint32_t            *channel_type);
} clap_plugin_cv_t;

typedef struct clap_host_cv {
   // Informs the host that the channels type have changed.
   // The channels type can only change when the plugin is de-activated.
   // [main-thread,!active]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_cv_t;

#ifdef __cplusplus
}
#endif
