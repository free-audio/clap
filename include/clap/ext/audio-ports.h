#pragma once

#include "../clap.h"
#include "../channel-map.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @page Audio Ports
///
/// This extension provides a way for the plugin to describe:
/// - its current ports
/// - possible ports configurations, for example mono, stereo, surround, ...
/// and a way for the host to select a configuration.
///
/// If the plugin does not implement this extension, it will have a stereo input and output.
///
/// During @ref clap_plugin.init, the plugin may query @ref clap_host_track_info and select a
/// configuration adapted to the track it belongs to.
///
/// After the plugin initialization, the host may scan the list of configurations and eventually
/// select one that fits the plugin context. The host can only select a configuration if the plugin
/// is deactivated.
///
/// A configuration is a very simple description of the audio ports:
/// - it describes the main input and output ports
/// - it has a name that can be displayed to the user
///
/// It is very easy for the host to offer a list of possible configurations and let the user choose
/// one.
///
/// Plugin with very complex configuration possibilities should let the user configure the ports
/// from the plugin GUI, and call @ref clap_host_audio_ports.rescan(CLAP_AUDIO_PORTS_RESCAN_ALL).

static const char CLAP_EXT_AUDIO_PORTS[] = "clap/audio-ports";

typedef struct clap_audio_port_info {
   clap_id id;                   // stable identifier
   char    name[CLAP_NAME_SIZE]; // displayable name, i18n?

   uint32_t   channel_count;
   clap_chmap channel_map;
   uint32_t   sample_size; // 32 for float and 64 for double

   bool is_main;  // there can only be 1 main input and output
   bool is_cv;    // control voltage
   bool in_place; // if true the daw can use the same buffer for input
                  // and output, only for main input to main output

} clap_audio_port_info;

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

// The audio ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_audio_ports {
   // number of ports, for either input or output
   // [main-thread]
   uint32_t (*port_count)(const clap_plugin *plugin, bool is_input);

   // get info about about an audio port.
   // [main-thread]
   bool (*port_info)(const clap_plugin *   plugin,
                     uint32_t              index,
                     bool                  is_input,
                     clap_audio_port_info *info);

   // gets the number of available configurations
   // [main-thread]
   uint32_t (*config_count)(const clap_plugin *plugin);

   // gets information about a configuration
   // [main-thread]
   bool (*config_info)(const clap_plugin *plugin, uint32_t index, clap_audio_ports_config *config);

   // selects the configuration designated by id
   // returns true if the configuration could be applied
   // [main-thread,plugin-deactivated]
   bool (*select_config)(const clap_plugin *plugin, clap_id config_id);
} clap_plugin_audio_ports;

enum {
   // The ports have changed, the host shall deactivate the plugin
   // and perform a full scan of the ports.
   CLAP_AUDIO_PORTS_RESCAN_ALL = 1 << 0,

   // The ports name did change, the host can scan them right away.
   CLAP_AUDIO_PORTS_RESCAN_NAMES = 1 << 1,

   // The list of configurations did change.
   CLAP_AUDIO_PORTS_RESCAN_CONFIGS = 1 << 2,
};

typedef struct clap_host_audio_ports {
   // [main-thread]
   uint32_t (*preferred_sample_size)(const clap_host *host);

   // Rescan the full list of audio ports according to the flags.
   // [main-thread]
   void (*rescan)(const clap_host *host, uint32_t flags);
} clap_host_audio_ports;

#ifdef __cplusplus
}
#endif
