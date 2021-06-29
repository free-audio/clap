#pragma once

#include <stdint.h>

#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_version {
   int major;
   int minor;
   int revision;
} clap_version;

static const clap_version CLAP_VERSION = {0, 7, 0};

static CLAP_CONSTEXPR bool clap_version_is_compatible(const clap_version& v)
{
   if (v.major == 0 && CLAP_VERSION.major == 0)
      return v.minor == CLAP_VERSION.major;

   if (v.major == 1 && CLAP_VERSION.major == 1)
      return true;

   return false;
}

#ifdef __cplusplus
}
#endif