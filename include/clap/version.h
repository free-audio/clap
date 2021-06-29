#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t clap_version;

static inline clap_version clap_version_make(uint32_t major, uint32_t minor, uint32_t revision) {
   return (((major & 0xfff) << 20) | ((minor & 0xfff) << 8) | (revision & 0xff));
}

static inline uint32_t clap_version_major(clap_version version)
{
   return ((version) >> 20) & 0xfff;
}

static inline uint32_t clap_version_minor(clap_version version)
{
   return ((version) >> 8) & 0xfff;
}

static inline uint32_t clap_version_revision(clap_version version)
{
   return version & 0xff;
}

static const clap_version CLAP_VERSION = clap_version_make(0, 7, 0);

#ifdef __cplusplus
}
#endif