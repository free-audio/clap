#pragma once

#include "../../plugin.h"

//usage examples:
//- the host can tell a MIDI keyboard the plugin conforms to the drawbar organ profile, so the keyboard can set up its faders for that.
//- the host can send per-note articulations in MIDI 2.0 protocol note-on attributes if there's an active profile for this.

//the 5 bytes repesenting a profile go in a uint64_t.
//the Default Control Change Mapping profile would be 0x00000000_0000217e for example.

//TODO : profiles can be different for each MIDI channel, and this should work per-input port.
//       we can add that once we've agreed on the basic functionality.
//TODO : for completeness it could work for outputs too. A MIDI effect plugin might want to know 
//       the host can receive per-note articulations in note attributes for example.

static CLAP_CONSTEXPR const char CLAP_EXT_MIDICI_PROFILES[] = "clap.midici-profiles.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_midici_profiles {
   // Returns the number of profiles supported.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Get a profile by index.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t         *plugin,
                       uint32_t                     profile_index,
                       uint64_t                    *profile
                       bool                        *active);

   // get number of bytes for profile specific data for the specified Inquiry Target
   // if result=0 there's no data available for this Inquiry Target
   // [main-thread]
   uint32_t(CLAP_ABI *get_data_size)(const clap_plugin_t         *plugin,
		                     uint32_t                     profile_index,
                                     uint8_t                      inquiry_target);

   // get profile specific data for the specified Inquiry Target
   // buffer must be large enough to contain the number of bytes returned by get_data_size()
   // [main-thread]
   bool(CLAP_ABI *get_data)(const clap_plugin_t         *plugin,
	                    uint32_t                     profile_index,
                            uint8_t                      inquiry_target,
                            const uint8_t               *buffer);

   // Actives a profile, so the plugin knows the host will use this
   // returns true if the profile was activated
   // Note: do not call clap_host_midici_profiles.changed
   // [main-thread]
   bool(CLAP_ABI *activate)(const clap_plugin_t    *plugin,
                            uint64_t                profile);

   // Deactives a profile
   // returns true if the profile was deactivated
   // Note: do not call clap_host_midici_profiles.changed
   // [main-thread]
   bool(CLAP_ABI *deactivate)(const clap_plugin_t  *plugin,
                              uint64_t              profile);
} clap_plugin_midici_profiles_t;

typedef struct clap_host_midici_profiles {
   // Informs the host that the available or active profiles changed.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_midici_profiles_t;

#ifdef __cplusplus
}
#endif