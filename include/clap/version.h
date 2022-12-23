#pragma once

#include "private/macros.h"
#include "private/std.h"

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

#define CLAP_VERSION_MAJOR_DIGITS 1
#define CLAP_VERSION_MINOR_DIGITS 1
#define CLAP_VERSION_REVISION_DIGITS 5

#define CLAP_VERSION_MAJOR ((uint32_t)CLAP_VERSION_MAJOR_DIGITS)
#define CLAP_VERSION_MINOR ((uint32_t)CLAP_VERSION_MINOR_DIGITS)
#define CLAP_VERSION_REVISION ((uint32_t)CLAP_VERSION_REVISION_DIGITS)

#define CLAP_VERSION_INIT                                                                          \
   { CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION }

// CLAP_VERSION_NUMBER can be used as a preprocessor directive comparator, for
// instance `#if CLAP_VERSION_NUMBER >= 0x010105` will be clap versions at or after 1.1.5
#define CLAP_VERSION_NUMBER ((CLAP_VERSION_MAJOR_DIGITS << 16) + (CLAP_VERSION_MINOR_DIGIT << 8) + CLAP_VERSION_REVISION_DIGIT)

static const CLAP_CONSTEXPR clap_version_t CLAP_VERSION = CLAP_VERSION_INIT;

CLAP_NODISCARD static inline CLAP_CONSTEXPR bool
clap_version_is_compatible(const clap_version_t v) {
   // versions 0.x.y were used during development stage and aren't compatible
   return v.major >= 1;
}


#if defined(__cplusplus) && __cplusplus >= 201703L
// Static assert some version constraints
static_assert(CLAP_VERSION_MAJOR_DIGITS < 256 && CLAP_VERSION_MAJOR_DIGITS >= 1);
static_assert(CLAP_VERSION_MINOR_DIGITS < 256 && CLAP_VERSION_REVISION >= 0);
static_assert(CLAP_VERSION_REVISION_DIGITS < 256 && CLAP_VERSION_REVISION_DIGITS >= 0);
#endif

