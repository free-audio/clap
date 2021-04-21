#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../clap.h"

#define CLAP_EXT_AUDIO_PORTS "clap/audio-ports"

enum clap_audio_port_channel_mapping {
   CLAP_AUDIO_PORT_UNSPECIFIED = 0,
   CLAP_AUDIO_PORT_MONO = 1,

   // left, right
   CLAP_AUDIO_PORT_STEREO = 2,

   // front left, front right, center, low, surround left, surround right
   // surround back left, surround back right
   CLAP_AUDIO_PORT_SURROUND = 3,
};

struct clap_audio_port_info {
   char    name[CLAP_NAME_SIZE]; // useful in the debugger
   bool    is_input;
   bool    is_main;
   int32_t channel_count;
   enum clap_audio_port_channel_mapping channel_mapping;
};

// The audio ports scan has to be done while the plugin is deactivated.
struct clap_plugin_audio_ports {
   // number of ports, both inputs and outputs
   // [main-thread]
   int32_t (*get_count)(struct clap_plugin *plugin);

   // get info about about an audio port.
   // [main-thread]
   void (*get_info)(struct clap_plugin *         plugin,
                    int32_t                      index,
                    struct clap_audio_port_info *info);

   // Returns the port latency.
   // [main-thread]
   int32_t (*get_latency)(struct clap_plugin *plugin, int32_t index);
};

struct clap_host_audio_ports {
   // Tell the host that the plugin ports has changed.
   // The host shall deactivate the plugin and then scan the ports again.
   // [main-thread]
   void (*changed)(struct clap_host *host, struct clap_plugin *plugin);

   // Tell the host that the latency changed.
   // The host should call get_port_latency on each ports.
   // [main-thread]
   void (*latency_changed)(struct clap_host *host, struct clap_plugin *plugin);
};

#ifdef __cplusplus
}
#endif
