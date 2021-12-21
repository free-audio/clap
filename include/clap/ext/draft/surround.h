#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_SURROUND[] = "clap.surround.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

enum {
   CLAP_SURROUND_FL = 0,   // Front Left
   CLAP_SURROUND_FR = 1,   // Front Right
   CLAP_SURROUND_FC = 2,   // Front Center
   CLAP_SURROUND_LFE = 3,  // Low Frequency
   CLAP_SURROUND_BL = 4,   // Back Left
   CLAP_SURROUND_BR = 5,   // Back Right
   CLAP_SURROUND_FLC = 6,  // Front Left of Center
   CLAP_SURROUND_FRC = 7,  // Front Right of Center
   CLAP_SURROUND_BC = 8,   // Back Center
   CLAP_SURROUND_SL = 9,   // Side Left
   CLAP_SURROUND_SR = 10,  // Side Right
   CLAP_SURROUND_TC = 11,  // Top Center
   CLAP_SURROUND_TFL = 12, // Front Left Height
   CLAP_SURROUND_TFC = 13, // Front Center Height
   CLAP_SURROUND_TFR = 14, // Front Right Height
   CLAP_SURROUND_TBL = 15, // Rear Left Height
   CLAP_SURROUND_TBC = 16, // Rear Center Height
   CLAP_SURROUND_TBR = 17, // Rear Right Height
};

typedef struct clap_plugin_surround {
   // [main-thread]
   uint32_t (*get_channel_type)(const clap_plugin_t *plugin,
                                bool                 is_input,
                                uint32_t             port_index,
                                uint32_t             channel_index);
} clap_plugin_surround_t;

typedef struct clap_host_surround {
   // Informs the host that the channel type have changed.
   // [main-thread]
   void (*changed)(const clap_host_t *host);
} clap_host_surround_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif