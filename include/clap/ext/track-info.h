#pragma once

#include "../plugin.h"
#include "../color.h"
#include "../string-sizes.h"

// This extension let the plugin query info about the track it's in.
// It is useful when the plugin is created, to initialize some parameters (mix, dry, wet)
// and pick a suitable configuration regarding audio port type and channel count.

static CLAP_CONSTEXPR const char CLAP_EXT_TRACK_INFO[] = "clap.track-info/1";

// The latest draft is 100% compatible.
// This compat ID may be removed in 2026.
static CLAP_CONSTEXPR const char CLAP_EXT_TRACK_INFO_COMPAT[] = "clap.track-info.draft/1";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_TRACK_INFO_HAS_TRACK_NAME = (1 << 0),
   CLAP_TRACK_INFO_HAS_TRACK_COLOR = (1 << 1),
   CLAP_TRACK_INFO_HAS_AUDIO_CHANNEL = (1 << 2),

   // This plugin is on a return track, initialize with wet 100%
   CLAP_TRACK_INFO_IS_FOR_RETURN_TRACK = (1 << 3),

   // This plugin is on a bus track, initialize with appropriate settings for bus processing
   CLAP_TRACK_INFO_IS_FOR_BUS = (1 << 4),

   // This plugin is on the master, initialize with appropriate settings for channel processing
   CLAP_TRACK_INFO_IS_FOR_MASTER = (1 << 5),
};

typedef struct clap_track_info {
   uint64_t flags; // see the flags above

   // track name, available if flags contain CLAP_TRACK_INFO_HAS_TRACK_NAME
   char name[CLAP_NAME_SIZE];

   // track color, available if flags contain CLAP_TRACK_INFO_HAS_TRACK_COLOR
   clap_color_t color;

   // available if flags contain CLAP_TRACK_INFO_HAS_AUDIO_CHANNEL
   // see audio-ports.h, struct clap_audio_port_info to learn how to use channel count and port type
   int32_t     audio_channel_count;
   const char *audio_port_type;
} clap_track_info_t;

typedef struct clap_plugin_track_info {
   // Called when the info changes.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_plugin_t *plugin);
} clap_plugin_track_info_t;

typedef struct clap_host_track_info {
   // Get info about the track the plugin belongs to.
   // Returns true on success and stores the result into info.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_host_t *host, clap_track_info_t *info);
} clap_host_track_info_t;

#ifdef __cplusplus
}
#endif
