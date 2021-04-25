#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_istream {
   void *ctx;

   /* returns the number of bytes read.
    * 0 for end of file.
    * -1 on error. */
   int64_t (*read)(clap_istream *stream, void *buffer, uint64_t size);
} clap_istream;

typedef struct clap_ostream {
   void *ctx;

   /* returns the number of bytes written.
    * -1 on error. */
   int64_t (*write)(clap_istream *stream, const void *buffer, uint64_t size);
} clap_ostream;

#ifdef __cplusplus
}
#endif