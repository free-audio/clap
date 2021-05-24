#pragma once

#include "../clap.h"
#include "../channel-map.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_AUDIO_PORTS "clap/audio-ports"

typedef struct clap_audio_port_info {
   clap_id id;                   // stable identifier
   char    name[CLAP_NAME_SIZE]; // displayable name, i18n?
   bool    is_main;              // there can only be 1 main input and output
   bool    is_cv;                // control voltage
   bool    supports_64_bits;     // 32 bit support is mandatory, the host chooses
                                 // between 32 and 64.
   bool supports_in_place;       // if true the daw can use the same buffer for input
                                 // and output, only for main input to main output
   int32_t    channel_count;
   clap_chmap channel_map;
} clap_audio_port_info;

// The audio ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_audio_ports {
   // number of ports, for either input or output
   // [main-thread]
   uint32_t (*count)(clap_plugin *plugin, bool is_input);

   // get info about about an audio port.
   // [main-thread]
   bool (*get_info)(clap_plugin *plugin, uint32_t index, bool is_input, clap_audio_port_info *info);
} clap_plugin_audio_ports;

enum {
   // The ports name did change, the host can scan them right away.
   CLAP_AUDIO_PORTS_RESCAN_NAMES = 1 << 0,

   // The ports have changed, the host shall deactivate the plugin
   // and perform a full scan of the ports.
   CLAP_AUDIO_PORTS_RESCAN_ALL = 1 << 1,
};

typedef struct clap_host_audio_ports {
   // Rescan the full list of audio ports according to the flags.
   // [main-thread]
   void (*rescan)(clap_host *host, uint32_t flags);
} clap_host_audio_ports;

#ifdef __cplusplus
}
#endif
