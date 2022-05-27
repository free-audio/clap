#pragma once

#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

// Commonly used hashing algorithms
enum {
   CLAP_HASH_CRC32 = 0,
   CLAP_HASH_CRC64 = 1,

   CLAP_HASH_MD5 = 1000,

   // SHA family
   CLAP_HASH_SHA1 = 2000,

   CLAP_HASH_SHA2_224 = 3000,
   CLAP_HASH_SHA2_256,
   CLAP_HASH_SHA2_384,
   CLAP_HASH_SHA2_512,
   CLAP_HASH_SHA2_512_224, // state: 512, digest: 224
   CLAP_HASH_SHA2_512_256, // state: 512, digest: 256

   CLAP_HASH_SHA3_224 = 4000,
   CLAP_HASH_SHA3_256,
   CLAP_HASH_SHA3_384,
   CLAP_HASH_SHA3_512,

   // BLAKE family
   CLAP_BLAKE2 = 20000,
   CLAP_BLAKE3 = 21000,
};
typedef uint32_t clap_hash;

#ifdef __cplusplus
}
#endif
