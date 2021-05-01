#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum clap_chmap {
   CLAP_CHMAP_UNSPECIFIED = 0,
   CLAP_CHMAP_MONO = 1,

   // left, right
   CLAP_CHMAP_STEREO = 2,

   // front left, front right, center, low, surround left, surround right
   // surround back left, surround back right
   CLAP_CHMAP_SURROUND = 3,
} clap_chmap;

#ifdef __cplusplus
}
#endif
