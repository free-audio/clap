#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_VERSION_MAKE(Major, Minor, Revision)                                                  \
   ((((Major)&0xff) << 16) | (((Minor)&0xff) << 8) | ((Revision)&0xff))
#define CLAP_VERSION CLAP_VERSION_MAKE(0, 4, 0)
#define CLAP_VERSION_MAJ(Version) (((Version) >> 16) & 0xff)
#define CLAP_VERSION_MIN(Version) (((Version) >> 8) & 0xff)
#define CLAP_VERSION_REV(Version) ((Version)&0xff)

#if defined _WIN32 || defined __CYGWIN__
#   ifdef __GNUC__
#      define CLAP_EXPORT __attribute__((dllexport))
#   else
#      define CLAP_EXPORT __declspec(dllexport)
#   endif
#else
#   if __GNUC__ >= 4 || defined(__clang__)
#      define CLAP_EXPORT __attribute__((visibility("default")))
#   else
#      define CLAP_EXPORT
#   endif
#endif

#ifdef __cplusplus
}
#endif