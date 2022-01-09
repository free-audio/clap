#pragma once

#include "../../plugin.h"

// This extension can be used to specify the channel mapping used by the plugin.

static CLAP_CONSTEXPR const char CLAP_EXT_AMBISONIC[] = "clap.ambisonic.draft/0";

static CLAP_CONSTEXPR const char CLAP_PORT_AMBISONIC[] = "ambisonic";

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

enum {
   // FuMa channel ordering
   CLAP_AMBISONIC_FUMA = 0,

   // ACN channel ordering
   CLAP_AMBISONIC_ACN = 1,
};

enum {
   CLAP_AMBISONIC_NORMALIZATION_MAXN = 0,
   CLAP_AMBISONIC_NORMALIZATION_SN3D = 1,
   CLAP_AMBISONIC_NORMALIZATION_N3D = 2,
   CLAP_AMBISONIC_NORMALIZATION_SN2D = 3,
   CLAP_AMBISONIC_NORMALIZATION_N2D = 4,
};

typedef struct clap_ambisonic_info {
   alignas(4) uint32_t ordering;
   alignas(4) uint32_t normalization;
} clap_ambisonic_info_t;

typedef struct clap_plugin_ambisonic {
   // Returns true on success
   // [main-thread]
   bool (*get_info)(const clap_plugin_t   *plugin,
                    bool                   is_input,
                    uint32_t               port_index,
                    clap_ambisonic_info_t *info);

} clap_plugin_ambisonic_t;

typedef struct clap_host_ambisonic {
   // Informs the host that the info have changed.
   // The info can only change when the plugin is de-activated.
   // [main-thread]
   void (*changed)(const clap_host_t *host);
} clap_host_ambisonic_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
