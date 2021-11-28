#pragma once

#include "../clap.h"
#include "../chmap.h"
#include "../string-sizes.h"

/// @page Audio Ports
///
/// This extension provides a way for the plugin to describe its current audio ports.
///
/// If the plugin does not implement this extension, it will have a default stereo input and output.
///
/// The plugin is only allowed to change its ports configuration while it is deactivated.

static CLAP_CONSTEXPR const char CLAP_EXT_AUDIO_PORTS[] = "clap.audio-ports";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_audio_port_info {
   clap_id id;                   // stable identifier
   char    name[CLAP_NAME_SIZE]; // displayable name

   uint32_t   channel_count;
   clap_chmap channel_map;
   uint32_t   sample_size; // 32 for float and 64 for double

   bool is_main;  // there can only be 1 main input and output
   bool is_cv;    // control voltage
   bool in_place; // if true the daw can use the same buffer for input
                  // and output, only for main input to main output

} clap_audio_port_info;

// The audio ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_audio_ports {
   // number of ports, for either input or output
   // [main-thread]
   uint32_t (*count)(const clap_plugin *plugin, bool is_input);

   // get info about about an audio port.
   // [main-thread]
   bool (*get)(const clap_plugin    *plugin,
               uint32_t              index,
               bool                  is_input,
               clap_audio_port_info *info);
} clap_plugin_audio_ports;

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
   // [main-thread]
   uint32_t (*get_preferred_sample_size)(const clap_host *host);

   // Rescan the full list of audio ports according to the flags.
   // [main-thread]
   void (*rescan)(const clap_host *host, uint32_t flags);
} clap_host_audio_ports;

#ifdef __cplusplus
}
#endif
