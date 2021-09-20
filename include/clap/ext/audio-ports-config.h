#pragma once

#include "../clap.h"
#include "../chmap.h"
#include "../string-sizes.h"

/// @page Audio Ports Config
///
/// This extension provides a way for the plugin to describe possible ports configurations, for
/// example mono, stereo, surround, ... and a way for the host to select a configuration.
///
/// After the plugin initialization, the host may scan the list of configurations and eventually
/// select one that fits the plugin context. The host can only select a configuration if the plugin
/// is deactivated.
///
/// A configuration is a very simple description of the audio ports:
/// - it describes the main input and output ports
/// - it has a name that can be displayed to the user
///
/// The idea behind the configurations, is to let the user choose one via a menu.
///
/// Plugin with very complex configuration possibilities should let the user configure the ports
/// from the plugin GUI, and call @ref clap_host_audio_ports.rescan(CLAP_AUDIO_PORTS_RESCAN_ALL).

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_CONFIG[] = "clap.audio-ports-config";

#ifdef __cplusplus
extern "C" {
#endif

// Minimalistic description of ports configuration
typedef struct clap_audio_ports_config {
   clap_id id;
   char    name[CLAP_NAME_SIZE];

   // main input info
   uint32_t   input_channel_count;
   clap_chmap input_channel_map;

   // main output info
   uint32_t   output_channel_count;
   clap_chmap output_channel_map;
} clap_audio_ports_config;

// The audio ports config scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_audio_ports_config {
   // gets the number of available configurations
   // [main-thread]
   uint32_t (*count)(const clap_plugin *plugin);

   // gets information about a configuration
   // [main-thread]
   bool (*get)(const clap_plugin *plugin, uint32_t index, clap_audio_ports_config *config);

   // selects the configuration designated by id
   // returns true if the configuration could be applied
   // [main-thread,plugin-deactivated]
   bool (*select)(const clap_plugin *plugin, clap_id config_id);
} clap_plugin_audio_ports_config;

typedef struct clap_host_audio_ports_config {
   // Rescan the full list of configs.
   // [main-thread]
   void (*rescan)(const clap_host *host);
} clap_host_audio_ports_config;

#ifdef __cplusplus
}
#endif
