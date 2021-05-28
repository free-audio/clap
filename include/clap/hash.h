#pragma once

#include "clap.h"

#ifdef __cplusplus
extern "C" {
#endif

// Commonly used hashing algorithms
enum {
   CLAP_HASH_CRC32,
   CLAP_HASH_CRC64,
   CLAP_HASH_MD5,
   CLAP_HASH_SHA1,
   CLAP_HASH_SHA2,
   CLAP_HASH_SHA3,
   CLAP_BLAKE2,
   CLAP_BLAKE3,
};
typedef uint32_t clap_hash;

#ifdef __cplusplus
}
#endif