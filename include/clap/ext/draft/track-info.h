#pragma once

#include "../../plugin.h"
#include "../../color.h"
#include "../../string-sizes.h"
#include "../../properties.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TRACK_INFO[] = "clap.track-info.draft/1";

// uses int64_t
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_PROP_ID[] = "clap.track-info.id";

// uses int64_t
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_PROP_INDEX[] = "clap.track-info.index";

// uses string
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_PROP_NAME[] = "clap.track-info.name";

// uses string
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_PROP_PATH[] = "clap.track-info.path";

// uses string
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_PROP_AUDIO_PORT_TYPE[] = "clap.track-info.audio_port_type";

// uses int64_t
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_PROP_AUDIO_CHANNEL_COUNT[] = "clap.track-info.audio_channel_count";

// uses clap_color_t
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_PROP_COLOR[] = "clap.track-info.color";

// uses bool
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_PROP_IS_RETURN_TRACK[] = "clap.track-info.is_return_track";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_track_info {
   // Informs the plugin that the given property changed.
   // If prop_id is null, then all properties are invalidated.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_plugin_t *plugin, const char *prop_id);
} clap_plugin_track_info_t;

typedef struct clap_host_track_info {
   // Properties must be queried on the main thread
   // [main-thread]
   const clap_properties_t *properties;
} clap_host_track_info_t;

#ifdef __cplusplus
}
#endif
