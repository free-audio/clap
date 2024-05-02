#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

/// @page Audio Ports
///
/// This extension provides a way for the plugin to describe its current audio ports.
///
/// If the plugin does not implement this extension, it won't have audio ports.
///
/// 32 bits support is required for both host and plugins. 64 bits audio is optional.
///
/// The plugin is only allowed to change its ports configuration while it is deactivated.

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS[] = "clap.audio-ports";
static CLAP_CONSTEXPR const char CLAP_PORT_MONO[] = "mono";
static CLAP_CONSTEXPR const char CLAP_PORT_STEREO[] = "stereo";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // This port is the main audio input or output.
   // There can be only one main input and main output.
   // Main port must be at index 0.
   CLAP_AUDIO_PORT_IS_MAIN = 1 << 0,

   // This port can be used with 64 bits audio
   CLAP_AUDIO_PORT_SUPPORTS_64BITS = 1 << 1,

   // 64 bits audio is preferred with this port
   CLAP_AUDIO_PORT_PREFERS_64BITS = 1 << 2,

   // This port must be used with the same sample size as all the other ports which have this flag.
   // In other words if all ports have this flag then the plugin may either be used entirely with
   // 64 bits audio or 32 bits audio, but it can't be mixed.
   CLAP_AUDIO_PORT_REQUIRES_COMMON_SAMPLE_SIZE = 1 << 3,
};

typedef struct clap_audio_port_info {
   // id identifies a port and must be stable.
   // id may overlap between input and output ports.
   clap_id id;
   char    name[CLAP_NAME_SIZE]; // displayable name

   uint32_t flags;
   uint32_t channel_count;

   // If null or empty then it is unspecified (arbitrary audio).
   // This field can be compared against:
   // - CLAP_PORT_MONO
   // - CLAP_PORT_STEREO
   // - CLAP_PORT_SURROUND (defined in the surround extension)
   // - CLAP_PORT_AMBISONIC (defined in the ambisonic extension)
   //
   // An extension can provide its own port type and way to inspect the channels.
   const char *port_type;

   // in-place processing: allow the host to use the same buffer for input and output
   // if supported set the pair port id.
   // if not supported set to CLAP_INVALID_ID
   clap_id in_place_pair;
} clap_audio_port_info_t;

// The audio ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_audio_ports {
   // Number of ports, for either input or output
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin, bool is_input);

   // Get info about an audio port.
   // Returns true on success and stores the result into info.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t    *plugin,
                       uint32_t                index,
                       bool                    is_input,
                       clap_audio_port_info_t *info);
} clap_plugin_audio_ports_t;

enum {
   // The ports name did change, the host can scan them right away.
   CLAP_AUDIO_PORTS_RESCAN_NAMES = 1 << 0,

   // [!active] The flags did change
   CLAP_AUDIO_PORTS_RESCAN_FLAGS = 1 << 1,

   // [!active] The channel_count did change
   CLAP_AUDIO_PORTS_RESCAN_CHANNEL_COUNT = 1 << 2,

   // [!active] The port type did change
   CLAP_AUDIO_PORTS_RESCAN_PORT_TYPE = 1 << 3,

   // [!active] The in-place pair did change, this requires.
   CLAP_AUDIO_PORTS_RESCAN_IN_PLACE_PAIR = 1 << 4,

   // [!active] The list of ports have changed: entries have been removed/added.
   CLAP_AUDIO_PORTS_RESCAN_LIST = 1 << 5,
};

typedef struct clap_host_audio_ports {
   // Checks if the host allows a plugin to change a given aspect of the audio ports definition.
   // [main-thread]
   bool(CLAP_ABI *is_rescan_flag_supported)(const clap_host_t *host, uint32_t flag);

   // Rescan the full list of audio ports according to the flags.
   // It is illegal to ask the host to rescan with a flag that is not supported.
   // Certain flags require the plugin to be de-activated.
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host, uint32_t flags);
} clap_host_audio_ports_t;

#ifdef __cplusplus
}
#endif
