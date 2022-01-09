#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

/// @page Audio Ports
///
/// This extension provides a way for the plugin to describe its current audio ports.
///
/// If the plugin does not implement this extension, it will have a default 32 bits stereo input and output.
/// This makes 32 bit support a requirement for both plugin and host.
///
/// The plugin is only allowed to change its ports configuration while it is deactivated.

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS[] = "clap.audio-ports";
static CLAP_CONSTEXPR const char CLAP_PORT_MONO[] = "mono";
static CLAP_CONSTEXPR const char CLAP_PORT_STEREO[] = "stereo";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

enum {
   // This port main audio input or output.
   // There can be only one main input and main output.
   CLAP_AUDIO_PORT_IS_MAIN = 1 << 0,

   // The prefers 64 bits audio with this port.
   CLAP_AUDIO_PORTS_PREFERS_64BITS = 1 << 1,
};

typedef struct clap_audio_port_info {
   alignas(4) clap_id id;                // stable identifier
   alignas(1) char name[CLAP_NAME_SIZE]; // displayable name

   alignas(4) uint32_t flags;
   alignas(4) uint32_t channel_count;

   // If null or empty then it is unspecified (arbitrary audio).
   // This filed can be compared against:
   // - CLAP_PORT_MONO
   // - CLAP_PORT_STEREO
   // - CLAP_PORT_SURROUND (defined in the surround extension)
   // - CLAP_PORT_AMBISONIC (defined in the ambisonic extension)
   // - CLAP_PORT_CV (defined in the cv extension)
   //
   // An extension can provide its own port type and way to inspect the channels.
   const char *port_type;

   // in-place processing: allow the host to use the same buffer for input and output
   // if supported set the pair port id.
   // if not supported set to CLAP_INVALID_ID
   alignas(4) clap_id in_place_pair;
} clap_audio_port_info_t;

// The audio ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_audio_ports {
   // number of ports, for either input or output
   // [main-thread]
   uint32_t (*count)(const clap_plugin_t *plugin, bool is_input);

   // get info about about an audio port.
   // [main-thread]
   bool (*get)(const clap_plugin_t    *plugin,
               uint32_t                index,
               bool                    is_input,
               clap_audio_port_info_t *info);
} clap_plugin_audio_ports_t;

enum {
   // The ports have changed, the host shall perform a full scan of the ports.
   // This flag can only be used if the plugin is not active.
   // If the plugin active, call host->request_restart() and then call rescan()
   // when the host calls deactivate()
   CLAP_AUDIO_PORTS_RESCAN_ALL = 1 << 0,

   // The ports name did change, the host can scan them right away.
   CLAP_AUDIO_PORTS_RESCAN_NAMES = 1 << 1,
};

typedef struct clap_host_audio_ports {
   // Rescan the full list of audio ports according to the flags.
   // [main-thread,!active]
   void (*rescan)(const clap_host_t *host, uint32_t flags);
} clap_host_audio_ports_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
