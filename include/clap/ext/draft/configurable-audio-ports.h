#pragma once

#include "../audio-ports.h"

#ifdef __cplusplus
extern "C" {
#endif

// This extension lets the host configure the plugin's input and output audio ports.
// This is a "push" approach to audio ports configuration.
static CLAP_CONSTEXPR const char CLAP_EXT_CONFIGURABLE_AUDIO_PORTS[] =
   "clap.configurable-audio-ports.draft0";

typedef struct clap_audio_port_configuration_request {
   // When true, allows the plugin to pick a similar port configuration instead
   // if the requested one can't be applied.
   bool is_best_effort;

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
   // - CLAP_PORT_AMBISONIC: const clap_ambisonic_info_t *info
   const void *port_details;
} clap_audio_port_configuration_request_t;

typedef struct clap_plugin_configurable_audio_ports {
   // Some ports may not be configurable, or simply the result of another port configuration.
   // For example if you have a simple delay plugin, then the output port must have the exact
   // same type as the input port; in that example, we consider the output port type to be a
   // function (identity) of the input port type.
   // [main-thread && !active]
   bool(CLAP_ABI *is_port_configurable)(const clap_plugin_t *plugin,
                                        bool                 is_input,
                                        uint32_t             port_index);

   // Submit a bunch of configuration requests which will atomically be applied together,
   // or discarded together.
   // [main-thread && !active]
   bool(CLAP_ABI *request_configuration)(
      const clap_plugin_t                                *plugin,
      const struct clap_audio_port_configuration_request *requests,
      uint32_t                                            request_count);
} clap_plugin_configurable_audio_ports_t;

#ifdef __cplusplus
}
#endif
