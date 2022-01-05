#pragma once

#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_CHMAP_UNSPECIFIED = 0,
   CLAP_CHMAP_MONO = 1,

   // left, right
   CLAP_CHMAP_STEREO = 2,

   // see clap_plugin_surround to inspect the exact channel layout
   CLAP_CHMAP_SURROUND = 3,

   // see clap_plugin_ambisonic to inspect the mapping
   CLAP_CHMAP_AMBISONIC = 4,
};
typedef int32_t clap_chmap;

#ifdef __cplusplus
}
#endif
