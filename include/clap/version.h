#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_version {
   // This is the major ABI and API design
   // Version 0.X.Y correspond to the development stage, API and ABI are not stable
   // Version 1.X.Y correspont to the release stage, API and ABI are stable
   uint32_t major;
   uint32_t minor;
   uint32_t revision;
} clap_version;

#ifdef __cplusplus
}
#endif

static CLAP_CONSTEXPR const clap_version CLAP_VERSION = {0, 15, 0};

static CLAP_CONSTEXPR inline bool clap_version_is_compatible(const clap_version v) {
   // For version 0, we require the same minor version because
   // we may still break the ABI at this point
   if (v.major == 0 && CLAP_VERSION.major == 0)
      return v.minor == CLAP_VERSION.minor;

   return false;
}