#pragma once

#include "../../clap.h"
#include "../audio-ports.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_TRACK_INFO "clap/draft/track-info"

typedef struct clap_color {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
   uint8_t alpha;
} clap_color;

typedef struct clap_track_info {
   uint32_t                        id;
   char                            name[CLAP_NAME_SIZE];
   int32_t                         track_index;
   char                            path[512]; // Like "/group1/group2/drum-machine/drum-pad"
   int32_t                         audio_channel_count;
   clap_audio_port_channel_mapping audio_channel_mapping;
   clap_color                      color;
   bool                            is_return_track;
} clap_track_info;

typedef struct clap_plugin_track_info {
   // [main-thread]
   int (*track_info_changed)(clap_plugin *plugin, const clap_track_info *info);
} clap_host_plugin_info;

typedef struct clap_host_track_info {
   // Informs the host that the note names has changed.
   // [main-thread]
   void (*get_track_info)(clap_host *host, clap_plugin *plugin, clap_track_info *info);
} clap_host_track_info;

#ifdef __cplusplus
}
#endif