#pragma once

#include "../../plugin.h"

// This extension can be used to specify the channel mapping used by the plugin.
// Work in progress, suggestions are welcome

static CLAP_CONSTEXPR const char CLAP_EXT_CV[] = "clap.cv.draft/0";
static CLAP_CONSTEXPR const char CLAP_PORT_CV[] = "cv";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

enum {
   // TODO: standardize values?
   CLAP_CV_GATE = 0,
   CLAP_CV_PITCH = 1,
   CLAP_CV_VALUE = 2,
};

typedef struct clap_plugin_cv {
   // Stores into the channel_map array, the surround identifer of each channels.
   // Returns the number of elements stored in channel_map
   // [main-thread]
   uint32_t (*get_channel_type)(const clap_plugin_t *plugin,
                                bool                 is_input,
                                uint32_t             port_index,
                                uint32_t             channel_index);
} clap_plugin_cv_t;

typedef struct clap_host_cv {
   // Informs the host that the channels type have changed.
   // The channels type can only change when the plugin is de-activated.
   // [main-thread,!active]
   void (*changed)(const clap_host_t *host);
} clap_host_cv_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
