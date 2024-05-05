#pragma once

#include "../../plugin.h"
#include "../../stream.h"

// usage examples:
// - the host can tell a MIDI keyboard the plugin conforms to the drawbar organ profile, so the keyboard can set up its faders for that.
// - the host can send per-note articulations in MIDI 2.0 protocol note-on attributes if there's an enabled profile for this.

// destination addressing:
// There are 4 types of profiles: single channel, multi channel, group and function block (called "port" here).
// The profile specifications define the type for a specific profile.
// A channel/num_channels pair allows for targeting profiles (used in profile_t, get_details() and enable()):
//   single-channel profiles: channel is 0..255, num_channels is 1
//   multi-channel profiles: channel is 0..255, num_channels is 1..256
//   group profiles: channel is 16 * group, num_channels is 0
//   port profiles: channel is 255, num_channels is 0

// enumerating profiles
// clap_plugin_midici_profiles.count()/get() return a list of clap_profile_t structs. Both enabled and available profiles are listed.
// All enabled profiles are listed. Multiple entries will appear if a channel or group profile is enabled on multiple channels or groups. 
// If a profile isn't enabled at least one CLAP_MIDICI_PROFILES_DISABLED entry appears in the list, so the host can detect the profile is supported.
// In case of a channel or group profile the host can try to enable it on any channel or group, and the plugin can reject this if it isn't possible.
// In detail:
//   enabled single channel profiles: one entry per enabled channel. At least one CLAP_MIDICI_PROFILES_DISABLED entry if the profile isn't enabled for any channel.
//   enabled multi channel profiles: one entry per enabled block of channels. At least one CLAP_MIDICI_PROFILES_DISABLED entry if the profile isn't enabled for any channel.
//   enabled group profiles: one entry per enabled group. At least one CLAP_MIDICI_PROFILES_DISABLED entry if the profile isn't enabled for any group.
//   port profiles: one entry.

// Plugins can typically use a simple fixed list. For example: in case of a single-channel profile and 16 channels, clap_plugin_midici_profiles.count() can always return 16.
// get() uses the current enabled state for each channel.

// A host will typically proceed in this order:
// 1. get list using clap_plugin_midici_profiles.count()/get().
// 2. enable/disable profiles as needed.
// 3. get profile details using clap_plugin_midici_profiles.get_details().

// Plugins can use CLAP_MIDICI_PROFILES_ALWAYSON for profiles which can't be enabled/disabled by the host.
// Note that a plugin can remove a CLAP_MIDICI_PROFILES_ALWAYSON profile and call clap_host_midici_profiles.changed().

// Some features of this extension can only be used with note ports using the CLAP_NOTE_DIALECT_MIDI2 dialect (groups in particular).

static CLAP_CONSTEXPR const char CLAP_EXT_MIDICI_PROFILES[] = "clap.midici-profiles/draft/1";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_MIDICI_PROFILES_DISABLED = 0,
   CLAP_MIDICI_PROFILES_ENABLED = 1,
   CLAP_MIDICI_PROFILES_ALWAYSON = 2
};

enum {
   CLAP_MIDICI_PROFILES_INQUIRY_TARGET_DATA = 255
};

enum {
   CLAP_MIDICI_PROFILES_ENABLE_SUCCES = -1,
   CLAP_MIDICI_PROFILES_ENABLE_FAILED = -2
};

typedef struct clap_profile_id {
   uint8_t id;
   uint8_t bank;
   uint8_t number;
   uint8_t version;
   uint8_t level;
} clap_profile_id_t;

typedef struct clap_profile {
   clap_profile_id_t profile_id;
   uint8_t           channel;        // see "destination addressing" paragraph at top of file.
   uint16_t          num_channels;   // see "destination addressing" paragraph at top of file.
   uint8_t           enabled;        // CLAP_MIDICI_PROFILES_*
} clap_profile_t;

typedef struct clap_plugin_midici_profiles {
   // Returns the number of clap_profile_t entries available.
   // The "enumerating profiles" paragraph above describes what's supposed to be in the list.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t   *plugin,
                             uint32_t               port_index);

   // Get a clap_profile_t by index.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t   *plugin,
                       uint32_t               port_index,
                       uint32_t               profile_index,
                       clap_profile_t        *profile);

   // Get profile details from profile at channel/num_channels for the specified inquiry_target.
   // Returns true if data is written to stream correctly.
   // Returns false if there's no data available for this inquiry_target.
   // channel and num-channels: see "destination addressing" paragraph at top of file.
   // inquiry_target is 0..127. CLAP_MIDICI_PROFILES_INQUIRY_TARGET_DATA can be used to get Profile Specific Data from the plugin.
   // Multiple Profile Specific Data Messages can be written to stream. Each one starts with 0xF0, followed by the actual Profile Specific Data, and ends with 0xF7.
   // Example: MPE Channel Response Type Notification would be F0 01 F7
   // The profile targeted by channel/num_channels should generally be enabled before calling this function.
   // In some cases profile detals like available channels are needed before enabling a profile. Plugins should make sure this is handled correctly.
   // [main-thread]
   bool(CLAP_ABI *get_details)(const clap_plugin_t       *plugin,
                               uint32_t                   port_index,
                               const clap_profile_id_t   *profile_id,
                               uint8_t                    channel,
                               uint16_t                   num_channels,
                               uint8_t                    inquiry_target,
                               const clap_ostream_t      *stream);

   // Send Profile Specific Data to profile at channel/num_channels.
   // Returns true if data was accepted
   // Multiple Profile Specific Data Messages can be passed in buffer. Each one starts with 0xF0, followed by the actual Profile Specific Data, and ends with 0xF7.
   // Example: MPE Channel Response Type Notification would be F0 01 F7
   // A profile may specify multiple data messages. It's recommended to send the complete 'state' in a single set_data() call.
   // [main-thread]
   bool(CLAP_ABI *set_data)(const clap_plugin_t       *plugin,
                            uint32_t                   port_index,
                            const clap_profile_id_t   *profile_id,
                            uint8_t                    channel,
                            uint16_t                   num_channels,
                            const uint8_t             *buffer,
                            uint32_t                   buffer_size);

   // Enables a profile at channel/num_channels.
   // Returns CLAP_MIDICI_PROFILES_ENABLE_SUCCES if the profile is enabled at channel/num_channels when the function returns.
   // Result >= 0 indicates a conflict with an existing enabled profile.
   // The host can call get(.., .., Result, ..), disable this profile and try again.
   // Returns CLAP_MIDICI_PROFILES_ENABLE_FAILED if an error other than a conflict occured.
   // Can be called multiple times to enabled a profile for multiple channels or groups.
   // channel and num-channels: see "destination addressing" paragraph at top of file.
   // Note for hosts: after calling this function count()/get() may have changed !!
   // Note for plugins: do not call clap_host_midici_profiles.changed. Do not disable profiles.
   // [main-thread]
   int32_t(CLAP_ABI *enable)(const clap_plugin_t       *plugin,
                             uint32_t                   port_index,
                             const clap_profile_id_t   *profile_id,
                             uint8_t                    channel,
                             uint16_t                   num_channels);

   // Disables a profile at channel/num_channels.
   // Returns true if the profile isn't enabled at channel/num_channels when the function returns.
   // Note for hosts: after calling this function count()/get() may have changed !!
   // Note for plugins: do not call clap_host_midici_profiles.changed.
   // [main-thread]
   bool(CLAP_ABI *disable)(const clap_plugin_t       *plugin,
                           uint32_t                   port_index,
                           const clap_profile_id_t   *profile_id,
                           uint8_t                    channel,
                           uint16_t                   num_channels);
} clap_plugin_midici_profiles_t;

typedef struct clap_host_midici_profiles {
   // Informs the host that the available or enabled profiles changed.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);

   // Plugins calls this if host needs to read Profile Specific Data Messages again.
   // Host calls get_details(.., port_index, profile, channel, num_channels, CLAP_MIDICI_PROFILES_INQUIRY_TARGET_DATA, ..).
   // [main-thread]
   void(CLAP_ABI *datachanged)(const clap_host_t         *host,
                               uint32_t                   port_index,
                               const clap_profile_id_t   *profile_id,
                               uint8_t                    channel,
                               uint16_t                   num_channels);
} clap_host_midici_profiles_t;

#ifdef __cplusplus
}
#endif
