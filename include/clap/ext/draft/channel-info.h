#pragma once

#include "../../plugin.h"
#include "../../color.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_CHANNEL_INFO[] = "clap.channel-info.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_CHANNEL_INFO_HAS_TRACK_NAME = (1 << 0),
   CLAP_CHANNEL_INFO_HAS_TRACK_COLOR = (1 << 1),
   CLAP_CHANNEL_INFO_HAS_TRACK_ID = (1 << 1),
   CLAP_CHANNEL_INFO_HAS_TRACK_PATH = (1 << 1),
   CLAP_CHANNEL_INFO_HAS_CHANNEL_NAME = (1 << 2),
   CLAP_CHANNEL_INFO_HAS_CHANNEL_COLOR = (1 << 3),
   CLAP_CHANNEL_INFO_HAS_CHANNEL_ID = (1 << 4),
   CLAP_CHANNEL_INFO_HAS_CHANNEL_PATH = (1 << 4),
   CLAP_CHANNEL_INFO_HAS_AUDIO_CHANNEL_COUNT = (1 << 5),
   CLAP_CHANNEL_INFO_HAS_AUDIO_PORT_TYPE = (1 << 6),
};

typedef struct clap_channel_info {
   uint32_t flags; // see the flags above

   // info relatives to the track to which the current channel belong
   char         track_id[CLAP_CUSTOM_ID_SIZE];
   char         track_name[CLAP_NAME_SIZE];
   char         track_path[CLAP_PATH_SIZE];
   clap_color_t track_color;
   bool         is_return_track;

   // info relatives to the channel owning the plugin instance
   // path can be like: "/group1/group2/track7/drum-machine/drum-pad-13"
   char channel_id[CLAP_CUSTOM_ID_SIZE];
   char channel_name[CLAP_NAME_SIZE];
   char channel_path[CLAP_PATH_SIZE];
   clap_color_t channel_color;
   bool         is_return_channel;

   int32_t     audio_channel_count;
   const char *audio_port_type;
} clap_channel_info_t;

typedef struct clap_plugin_channel_info {
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_plugin_t *plugin);
} clap_plugin_channel_info_t;

typedef struct clap_host_channel_info {
   // Get info about the track the plugin belongs to.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_host_t *host, clap_channel_info_t *info);
} clap_host_channel_info_t;

#ifdef __cplusplus
}
#endif
