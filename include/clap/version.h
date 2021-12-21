#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "private/macros.h"

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
} clap_version_t;

#ifdef __cplusplus
}
#endif

#define CLAP_VERSION_MAJOR ((uint32_t)0)
#define CLAP_VERSION_MINOR ((uint32_t)16)
#define CLAP_VERSION_REVISION ((uint32_t)0)

#ifdef __cplusplus
#   define CLAP_VERSION                                                                            \
      (clap_version_t{CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION})
#else
#   define CLAP_VERSION                                                                            \
      ((clap_version_t){CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION})
#endif

// For version 0, we require the same minor version because
// we may still break the ABI at this point
#define clap_version_is_compatible(Version)                                                        \
   ((Version).major == CLAP_VERSION_MAJOR && (Version).minor == CLAP_VERSION_MINOR)
