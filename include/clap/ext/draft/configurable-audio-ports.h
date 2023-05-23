#pragma once

#include "../audio-ports.h"

#ifdef __cplusplus
extern "C" {
#endif

// This extension lets the host configure the plugin's input and output audio ports.
// This is a "push" approach to audio ports configuration.
static CLAP_CONSTEXPR const char CLAP_EXT_CONFIGURABLE_AUDIO_PORTS[] =
   "clap.configurable-audio-ports.draft1";

typedef struct clap_audio_port_configuration_request {
   // Identifies the port by is_input and port_index
   bool     is_input;
   uint32_t port_index;

   // The requested number of channels.
   uint32_t channel_count;

   // The port type, see audio-ports.h, clap_audio_port_info.port_type for interpretation.
   const char *port_type;

   // cast port_details according to port_type:
   // - CLAP_PORT_MONO: (discard)
   // - CLAP_PORT_STEREO: (discard)
   // - CLAP_PORT_SURROUND: const uint8_t *channel_map
   // - CLAP_PORT_AMBISONIC: const clap_ambisonic_config_t *info
   const void *port_details;
} clap_audio_port_configuration_request_t;

typedef struct clap_plugin_configurable_audio_ports {
   // If is_dry_run is true, then checks if the configuration can be applied.
   // If is_dry_run is false, then applies the configuration.
   // Returns true if applied.
   // [main-thread && !active]
   bool(CLAP_ABI *apply_configuration)(const clap_plugin_t                                *plugin,
                                       const struct clap_audio_port_configuration_request *requests,
                                       uint32_t request_count,
                                       bool     is_dry_run);
} clap_plugin_configurable_audio_ports_t;

#ifdef __cplusplus
}
#endif
