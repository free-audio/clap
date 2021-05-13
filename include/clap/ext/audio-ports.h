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
   int32_t (*get_count)(clap_plugin *plugin, bool is_input);

   // get info about about an audio port.
   // [main-thread]
   void (*get_info)(clap_plugin *plugin, int32_t index, bool is_input, clap_audio_port_info *info);

   void (*set_active)(
      clap_plugin *plugin, int32_t index, bool is_input, bool use_64, bool is_active);
} clap_plugin_audio_ports;

typedef struct clap_host_audio_ports {
   // Tell the host that the plugin ports has changed.
   // The host shall deactivate the plugin and then scan the ports again.
   // [main-thread]
   void (*changed)(clap_host *host);

   // Tell the host that the plugin ports name have changed.
   // It is not necessary to deactivates the plugin.
   // [main-thread]
   void (*name_changed)(clap_host *host);
} clap_host_audio_ports;

#ifdef __cplusplus
}
#endif
