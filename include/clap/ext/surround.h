#pragma once

#include "../plugin.h"

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

static CLAP_CONSTEXPR const char CLAP_EXT_SURROUND[] = "clap.surround/4";

// The latest draft is 100% compatible.
// This compat ID may be removed in 2026.
static CLAP_CONSTEXPR const char CLAP_EXT_SURROUND_COMPAT[] = "clap.surround.draft/4";

static CLAP_CONSTEXPR const char CLAP_PORT_SURROUND[] = "surround";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_SURROUND_FL = 0,   // Front Left
   CLAP_SURROUND_FR = 1,   // Front Right
   CLAP_SURROUND_FC = 2,   // Front Center
   CLAP_SURROUND_LFE = 3,  // Low Frequency
   CLAP_SURROUND_BL = 4,   // Back (Rear) Left
   CLAP_SURROUND_BR = 5,   // Back (Rear) Right
   CLAP_SURROUND_FLC = 6,  // Front Left of Center
   CLAP_SURROUND_FRC = 7,  // Front Right of Center
   CLAP_SURROUND_BC = 8,   // Back (Rear) Center
   CLAP_SURROUND_SL = 9,   // Side Left
   CLAP_SURROUND_SR = 10,  // Side Right
   CLAP_SURROUND_TC = 11,  // Top (Height) Center
   CLAP_SURROUND_TFL = 12, // Top (Height) Front Left
   CLAP_SURROUND_TFC = 13, // Top (Height) Front Center
   CLAP_SURROUND_TFR = 14, // Top (Height) Front Right
   CLAP_SURROUND_TBL = 15, // Top (Height) Back (Rear) Left
   CLAP_SURROUND_TBC = 16, // Top (Height) Back (Rear) Center
   CLAP_SURROUND_TBR = 17, // Top (Height) Back (Rear) Right
   CLAP_SURROUND_TSL = 18, // Top (Height) Side Left
   CLAP_SURROUND_TSR = 19, // Top (Height) Side Right
};

typedef struct clap_plugin_surround {
   // Checks if a given channel mask is supported.
   // The channel mask is a bitmask, for example:
   //   (1 << CLAP_SURROUND_FL) | (1 << CLAP_SURROUND_FR) | ...
   // [main-thread]
   bool(CLAP_ABI *is_channel_mask_supported)(const clap_plugin_t *plugin, uint64_t channel_mask);

   // Stores the surround identifier of each channel into the channel_map array.
   // Returns the number of elements stored in channel_map.
   // channel_map_capacity must be greater or equal to the channel count of the given port.
   // [main-thread]
   uint32_t(CLAP_ABI *get_channel_map)(const clap_plugin_t *plugin,
                                       bool                 is_input,
                                       uint32_t             port_index,
                                       uint8_t             *channel_map,
                                       uint32_t             channel_map_capacity);
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
