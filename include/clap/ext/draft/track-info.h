#pragma once

#include "../../clap.h"
#include "../../channel-map.h"
#include "../../color.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_TRACK_INFO "clap/draft/track-info"

typedef struct clap_track_info {
   clap_id    id;
   int32_t    index;
   char       name[CLAP_NAME_SIZE];
   char       path[512]; // Like "/group1/group2/drum-machine/drum-pad"
   int32_t    channel_count;
   clap_chmap channel_map;
   clap_color color;
   bool       is_return_track;
} clap_track_info;

typedef struct clap_plugin_track_info {
   // [main-thread]
   void (*track_info_changed)(clap_plugin *plugin);
} clap_plugin_track_info;

typedef struct clap_host_track_info {
   // Informs the host that the note names has changed.
   // [main-thread]
   bool (*get_track_info)(clap_host *host, clap_track_info *info);
} clap_host_track_info;

#ifdef __cplusplus
}
#endif