#pragma once

#include "../string-sizes.h"
#include "../plugin.h"
#include "audio-ports.h"

/// @page Audio Ports Config
///
/// This extension let the plugin provide port configurations presets.
/// For example mono, stereo, surround, ambisonic, ...
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
/// Plugins with very complex configuration possibilities should let the user configure the ports
/// from the plugin GUI, and call @ref clap_host_audio_ports.rescan(CLAP_AUDIO_PORTS_RESCAN_ALL).
///
/// To inquire the exact bus layout, the plugin implements the clap_plugin_audio_ports_config_info_t
/// extension where all busses can be retrieved in the same way as in the audio-port extension.

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_CONFIG[] = "clap.audio-ports-config";

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_CONFIG_INFO[] =
   "clap.audio-ports-config-info/1";

// The latest draft is 100% compatible.
// This compat ID may be removed in 2026.
static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS_CONFIG_INFO_COMPAT[] =
   "clap.audio-ports-config-info/draft-0";

#ifdef __cplusplus
extern "C" {
#endif

// Minimalistic description of ports configuration
typedef struct clap_audio_ports_config {
   clap_id id;
   char    name[CLAP_NAME_SIZE];

   uint32_t input_port_count;
   uint32_t output_port_count;

   // main input info
   bool        has_main_input;
   uint32_t    main_input_channel_count;
   const char *main_input_port_type;

   // main output info
   bool        has_main_output;
   uint32_t    main_output_channel_count;
   const char *main_output_port_type;
} clap_audio_ports_config_t;

// The audio ports config scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_audio_ports_config {
   // Gets the number of available configurations
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Gets information about a configuration
   // Returns true on success and stores the result into config.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t       *plugin,
                       uint32_t                   index,
                       clap_audio_ports_config_t *config);

   // Selects the configuration designated by id
   // Returns true if the configuration could be applied.
   // Once applied the host should scan again the audio ports.
   // [main-thread & plugin-deactivated]
   bool(CLAP_ABI *select)(const clap_plugin_t *plugin, clap_id config_id);
} clap_plugin_audio_ports_config_t;

// Extended config info
typedef struct clap_plugin_audio_ports_config_info {

   // Gets the id of the currently selected config, or CLAP_INVALID_ID if the current port
   // layout isn't part of the config list.
   //
   // [main-thread]
   clap_id(CLAP_ABI *current_config)(const clap_plugin_t *plugin);

   // Get info about an audio port, for a given config_id.
   // This is analogous to clap_plugin_audio_ports.get().
   // Returns true on success and stores the result into info.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t    *plugin,
                       clap_id                 config_id,
                       uint32_t                port_index,
                       bool                    is_input,
                       clap_audio_port_info_t *info);
} clap_plugin_audio_ports_config_info_t;

typedef struct clap_host_audio_ports_config {
   // Rescan the full list of configs.
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host);
} clap_host_audio_ports_config_t;

#ifdef __cplusplus
}
#endif
