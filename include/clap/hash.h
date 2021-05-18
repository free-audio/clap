#pragma once

#include "clap.h"

#ifdef __cplusplus
extern "C" {
#endif

// Commonly used hashing algorithms
enum {
    // 32 bits
    CLAP_HASH_CRC32,

    // 64 bits
    CLAP_HASH_CRC64,

    // 128 bits
    CLAP_HASH_MD5,

    // 160 bits
    CLAP_HASH_SHA1,

    // 256 bits
    CLAP_HASH_SHA2,

    // 512 bits
    CLAP_HASH_SHA3,
};
typedef uint32_t clap_hash;

#ifdef __cplusplus
}
#endif