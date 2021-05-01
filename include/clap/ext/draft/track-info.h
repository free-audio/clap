#pragma once

#include "../../channel-map.h"
#include "../../clap.h"
#include "../../color.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_TRACK_INFO "clap/draft/track-info"

typedef struct clap_track_info {
   uint32_t   id;
   char       name[CLAP_NAME_SIZE];
   int32_t    track_index;
   char       path[512]; // Like "/group1/group2/drum-machine/drum-pad"
   int32_t    channel_count;
   clap_chmap channel_map;
   clap_color color;
   bool       is_return_track;
} clap_track_info;

typedef struct clap_plugin_track_info {
   // [main-thread]
   int (*track_info_changed)(clap_plugin *plugin, const clap_track_info *info);
} clap_plugin_track_info;

typedef struct clap_host_track_info {
   // Informs the host that the note names has changed.
   // [main-thread]
   void (*get_track_info)(clap_host *host, clap_plugin *plugin, clap_track_info *info);
} clap_host_track_info;

#ifdef __cplusplus
}
#endif