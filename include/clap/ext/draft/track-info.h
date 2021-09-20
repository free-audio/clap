#pragma once

#include "../../clap.h"
#include "../../chmap.h"
#include "../../color.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TRACK_INFO[] = "clap.track-info.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_track_info {
   clap_id    id;
   int32_t    index;
   char       name[CLAP_NAME_SIZE];
   char       path[512]; // Like "/group1/group2/drum-machine/drum-pad-13"
   int32_t    channel_count;
   clap_chmap channel_map;
   clap_color color;
   bool       is_return_track;
} clap_track_info;

typedef struct clap_plugin_track_info {
   // [main-thread]
   void (*changed)(const clap_plugin *plugin);
} clap_plugin_track_info;

typedef struct clap_host_track_info {
   // Get info about the track the plugin belongs to.
   // [main-thread]
   bool (*get)(const clap_host *host, clap_track_info *info);
} clap_host_track_info;

#ifdef __cplusplus
}
#endif