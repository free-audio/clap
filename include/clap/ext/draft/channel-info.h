#pragma once

#include "../../plugin.h"
#include "../../color.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_CHANNEL_INFO[] = "clap.channel-info.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_CHANNEL_INFO_HAS_TRACK_ID = (1 << 0),
   CLAP_CHANNEL_INFO_HAS_TRACK_NAME = (1 << 1),
   CLAP_CHANNEL_INFO_HAS_TRACK_PATH = (1 << 2),
   CLAP_CHANNEL_INFO_HAS_TRACK_COLOR = (1 << 3),
   CLAP_CHANNEL_INFO_HAS_DEVICE_CHAIN_ID = (1 << 4),
   CLAP_CHANNEL_INFO_HAS_DEVICE_CHAIN_NAME = (1 << 5),
   CLAP_CHANNEL_INFO_HAS_DEVICE_CHAIN_PATH = (1 << 6),
   CLAP_CHANNEL_INFO_HAS_DEVICE_CHAIN_COLOR = (1 << 7),
   CLAP_CHANNEL_INFO_HAS_AUDIO_CHANNEL_COUNT = (1 << 8),
   CLAP_CHANNEL_INFO_HAS_AUDIO_PORT_TYPE = (1 << 9),
};

typedef struct clap_channel_info {
   uint32_t flags; // see the flags above

   // Info relatives to the track to which the current channel belong
   // track_id: a host specific track identifier, can be a uuid for example
   // track_name: the track's name, eg: "808 Legend"
   // track_path: the path to the track, eg: "/group1/group2/808 Legend"
   // track_color: the track's color
   // is_return_track: true if the track is a return track, in which case you may want to initialize
   //    your FX's 100% wet (no dry signal)
   char         track_id[CLAP_CUSTOM_ID_SIZE];
   char         track_name[CLAP_NAME_SIZE];
   char         track_path[CLAP_PATH_SIZE];
   clap_color_t track_color;
   bool         is_return_track;

   // Info relatives to the device chain owning the plugin instance.
   // A track is a channel.
   // A channel contains one device chain and usually a mixer at the end.
   // A device chain, contains a bunch of devices.
   // A device may contain nested channels, return channels (for send FX) and device chains.
   // For example, consider a Drum Machine device, each drum pads contains a channel which contains
   // themselves device chains. Additionnaly a Drum Machine device can contain return channels for
   // send FX.
   //
   // device_chain_id: a host specific device_chain identifier, can be a uuid for example
   // device_chain_path: "/group1/group2/808 Legend/drum-machine/pads/808 Legend CLAP"
   //                 or "/group1/group2/808 Legend/drum-machine/returns/Delay"
   // device_chain_color: the device chain's color, in our example, that could be the drum pad
   // color. is_return_channel: true if the channel owning the plugin is a return channel, in which
   //    case you may want to initialize your FX's 100% wet (no dry signal)
   char         device_chain_id[CLAP_CUSTOM_ID_SIZE];
   char         device_chain_name[CLAP_NAME_SIZE];
   char         device_chain_path[CLAP_PATH_SIZE];
   clap_color_t device_chain_color;
   bool         is_return_channel;

   // see audio-ports.h, struct clap_audio_port_info to learn how to use channel count and port type
   int32_t     audio_channel_count;
   const char *audio_port_type;
} clap_channel_info_t;

typedef struct clap_plugin_channel_info {
   // Called when the channel info changes.
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
