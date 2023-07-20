#pragma once

#include "../../plugin.h"

//usage examples:
//- the host can tell a MIDI keyboard the plugin conforms to the drawbar organ profile, so the keyboard can set up its faders for that.
//- the plugin can adapt its MIDI controller mapping if a MIDI keyboard actives a profile (via the host).
//- the host can send per-note articulations in note attributes if there's an active profile for this.

//the 5 bytes repesenting a profile go in a uint64_t.
//the Default Control Change Mapping profile would be 0x00000000_0000217e for example.

//TODO : for completeness this should work per-input port. Is that worth it?
//TODO : for even more completeness it could work for outputs too. A MIDI effect plugin might want to know 
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

   //Actives a profile
   //returns true if the profile was activated
   //TODO : should this call struct clap_host_midici_profiles.changed or not?
   bool(CLAP_ABI *activate)(const clap_plugin_t    *plugin,
                            uint64_t                profile);

   //Deactives a profile
   //returns true if the profile was deactivated
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