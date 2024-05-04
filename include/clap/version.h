#pragma once

#include "private/macros.h"
#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_version {
   // This is the major ABI and API design
   // Version 0.X.Y correspond to the development stage, API and ABI are not stable
   // Version 1.X.Y correspond to the release stage, API and ABI are stable
   uint32_t major;
   uint32_t minor;
   uint32_t revision;
} clap_version_t;

#ifdef __cplusplus
}
#endif

#define CLAP_VERSION_MAJOR 1
#define CLAP_VERSION_MINOR 2
#define CLAP_VERSION_REVISION 1

#define CLAP_VERSION_INIT                                                                          \
   { (uint32_t)CLAP_VERSION_MAJOR, (uint32_t)CLAP_VERSION_MINOR, (uint32_t)CLAP_VERSION_REVISION }

#define CLAP_VERSION_LT(maj,min,rev) ((CLAP_VERSION_MAJOR < (maj)) || \
                    ((maj) == CLAP_VERSION_MAJOR && CLAP_VERSION_MINOR < (min)) || \
                    ((maj) == CLAP_VERSION_MAJOR && (min) == CLAP_VERSION_MINOR && CLAP_VERSION_REVISION < (rev)))
#define CLAP_VERSION_EQ(maj,min,rev) (((maj) == CLAP_VERSION_MAJOR) && ((min) == CLAP_VERSION_MINOR) && ((rev) == CLAP_VERSION_REVISION))
#define CLAP_VERSION_GE(maj,min,rev) (!CLAP_VERSION_LT(maj,min,rev))

static const CLAP_CONSTEXPR clap_version_t CLAP_VERSION = CLAP_VERSION_INIT;

CLAP_NODISCARD static inline CLAP_CONSTEXPR bool
clap_version_is_compatible(const clap_version_t v) {
   // versions 0.x.y were used during development stage and aren't compatible
   return v.major >= 1;
}
