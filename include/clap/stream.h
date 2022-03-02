#pragma once

#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_istream {
   void *ctx; // reserved pointer for the stream

   /* returns the number of bytes read.
    * 0 for end of file.
    * -1 on error. */
   int64_t (*read)(struct clap_istream *stream, void *buffer, uint64_t size);
} clap_istream_t;

typedef struct clap_ostream {
   void *ctx; // reserved pointer for the stream

   /* returns the number of bytes written.
    * -1 on error. */
   int64_t (*write)(struct clap_ostream *stream, const void *buffer, uint64_t size);
} clap_ostream_t;

#ifdef __cplusplus
}
#endif
