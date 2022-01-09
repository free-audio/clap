#pragma once

#include "../../plugin.h"
#include "../../color.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TRACK_INFO[] = "clap.track-info.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_track_info {
   alignas(4) clap_id      id;
   alignas(4) int32_t      index;
   alignas(1) char         name[CLAP_NAME_SIZE];
   alignas(1) char         path[CLAP_MODULE_SIZE]; // Like "/group1/group2/drum-machine/drum-pad-13"
   alignas(4) int32_t      channel_count;
   const char             *audio_port_type;
   alignas(4) clap_color_t color;
   alignas(4) bool         is_return_track;
} clap_track_info_t;

typedef struct clap_plugin_track_info {
   // [main-thread]
   void (*changed)(const clap_plugin_t *plugin);
} clap_plugin_track_info_t;

typedef struct clap_host_track_info {
   // Get info about the track the plugin belongs to.
   // [main-thread]
   bool (*get)(const clap_host_t *host, clap_track_info_t *info);
} clap_host_track_info_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif