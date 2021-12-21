#pragma once

#include "private/macros.h"
#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_version {
   // This is the major ABI and API design
   // Version 0.X.Y correspond to the development stage, API and ABI are not stable
   // Version 1.X.Y correspont to the release stage, API and ABI are stable
   alignas(4) uint32_t major;
   alignas(4) uint32_t minor;
   alignas(4) uint32_t revision;
} clap_version_t;

#pragma pack(pop)

static const uint32_t CLAP_VERSION_MAJOR = 0;
static const uint32_t CLAP_VERSION_MINOR = 16;
static const uint32_t CLAP_VERSION_REVISION = 0;

static const clap_version_t CLAP_VERSION = {
   CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION};

// For version 0, we require the same minor version because
// we may still break the ABI at this point
static inline bool clap_version_is_compatible(const clap_version_t v) {
   return v.major == CLAP_VERSION_MAJOR && v.minor == CLAP_VERSION_MINOR;
}

#ifdef __cplusplus
}
#endif