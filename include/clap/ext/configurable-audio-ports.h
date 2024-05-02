#pragma once

#include "audio-ports.h"

#ifdef __cplusplus
extern "C" {
#endif

// This extension lets the host configure the plugin's input and output audio ports.
// This is a "push" approach to audio ports configuration.

static CLAP_CONSTEXPR const char CLAP_EXT_CONFIGURABLE_AUDIO_PORTS[] =
   "clap.configurable-audio-ports/1";

// The latest draft is 100% compatible.
// This compat ID may be removed in 2026.
static CLAP_CONSTEXPR const char CLAP_EXT_CONFIGURABLE_AUDIO_PORTS_COMPAT[] =
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
   // Returns true if the given configurations can be applied using apply_configuration().
   // [main-thread && !active]
   bool(CLAP_ABI *can_apply_configuration)(
      const clap_plugin_t                                *plugin,
      const struct clap_audio_port_configuration_request *requests,
      uint32_t                                            request_count);

   // Submit a bunch of configuration requests which will atomically be applied together,
   // or discarded together.
   //
   // Once the configuration is successfully applied, it isn't necessary for the plugin to call
   // clap_host_audio_ports->changed(); and it isn't necessary for the host to scan the
   // audio ports.
   //
   // Returns true if applied.
   // [main-thread && !active]
   bool(CLAP_ABI *apply_configuration)(const clap_plugin_t                                *plugin,
                                       const struct clap_audio_port_configuration_request *requests,
                                       uint32_t request_count);
} clap_plugin_configurable_audio_ports_t;

#ifdef __cplusplus
}
#endif
