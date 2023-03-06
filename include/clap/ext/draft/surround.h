#pragma once

#include "../../plugin.h"

// This extension can be used to specify the channel mapping used by the plugin.
//
// To have consistent surround features across all the plugin instances,
// here is the proposed workflow:
// 1. the plugin queries the host preferred channel mapping and
//    adjusts its configuration to match it.
// 2. the host checks how the plugin is effectively configured and honors it.
//
// If the host decides to change the project's surround setup:
// 1. deactivate the plugin
// 2. host calls clap_plugin_surround->changed()
// 3. plugin calls clap_host_surround->get_preferred_channel_map()
// 4. plugin eventually calls clap_host_surround->changed()
// 5. host calls clap_plugin_surround->get_channel_map() if changed
// 6. host activates the plugin and can start processing audio
//
// If the plugin wants to change its surround setup:
// 1. call host->request_restart() if the plugin is active
// 2. once deactivated plugin calls clap_host_surround->changed()
// 3. host calls clap_plugin_surround->get_channel_map()
// 4. host activates the plugin and can start processing audio

static CLAP_CONSTEXPR const char CLAP_EXT_SURROUND[] = "clap.surround.draft/3";

static CLAP_CONSTEXPR const char CLAP_PORT_SURROUND[] = "surround";

#ifdef __cplusplus
extern "C" {
#endif

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
   // Stores into the channel_map array, the surround identifier of each channel.
   // Returns the number of elements stored in channel_map.
   //
   // config_id: the configuration id, see clap_plugin_audio_ports_config.
   // If config_id is CLAP_INVALID_ID, then this function queries the current port info.
   // [main-thread]
   uint32_t(CLAP_ABI *get_channel_map)(const clap_plugin_t *plugin,
                                       bool                 is_input,
                                       uint32_t             port_index,
                                       uint8_t             *channel_map,
                                       uint32_t             channel_map_capacity);

   // Informs the plugin that the host preferred channel map has changed.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_plugin_t *plugin);
} clap_plugin_surround_t;

typedef struct clap_host_surround {
   // Informs the host that the channel map has changed.
   // The channel map can only change when the plugin is de-activated.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_surround_t;

#ifdef __cplusplus
}
#endif
