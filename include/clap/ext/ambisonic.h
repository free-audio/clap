#pragma once

#include "../plugin.h"

// This extension can be used to specify the channel mapping used by the plugin.
static CLAP_CONSTEXPR const char CLAP_EXT_AMBISONIC[] = "clap.ambisonic/3";

// The latest draft is 100% compatible.
// This compat ID may be removed in 2026.
static CLAP_CONSTEXPR const char CLAP_EXT_AMBISONIC_COMPAT[] = "clap.ambisonic.draft/3";

static CLAP_CONSTEXPR const char CLAP_PORT_AMBISONIC[] = "ambisonic";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_ambisonic_ordering {
   // FuMa channel ordering
   CLAP_AMBISONIC_ORDERING_FUMA = 0,

   // ACN channel ordering
   CLAP_AMBISONIC_ORDERING_ACN = 1,
};

enum clap_ambisonic_normalization {
   CLAP_AMBISONIC_NORMALIZATION_MAXN = 0,
   CLAP_AMBISONIC_NORMALIZATION_SN3D = 1,
   CLAP_AMBISONIC_NORMALIZATION_N3D = 2,
   CLAP_AMBISONIC_NORMALIZATION_SN2D = 3,
   CLAP_AMBISONIC_NORMALIZATION_N2D = 4,
};

typedef struct clap_ambisonic_config {
   uint32_t ordering;      // see clap_ambisonic_ordering
   uint32_t normalization; // see clap_ambisonic_normalization
} clap_ambisonic_config_t;

typedef struct clap_plugin_ambisonic {
   // Returns true if the given configuration is supported.
   // [main-thread]
   bool(CLAP_ABI *is_config_supported)(const clap_plugin_t           *plugin,
                                       const clap_ambisonic_config_t *config);

   // Returns true on success
   //
   // config_id: the configuration id, see clap_plugin_audio_ports_config.
   // If config_id is CLAP_INVALID_ID, then this function queries the current port info.
   // [main-thread]
   bool(CLAP_ABI *get_config)(const clap_plugin_t     *plugin,
                              bool                     is_input,
                              uint32_t                 port_index,
                              clap_ambisonic_config_t *config);

} clap_plugin_ambisonic_t;

typedef struct clap_host_ambisonic {
   // Informs the host that the info has changed.
   // The info can only change when the plugin is de-activated.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_ambisonic_t;

#ifdef __cplusplus
}
#endif
