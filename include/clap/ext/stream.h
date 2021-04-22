#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct clap_istream {
   void *ctx;

   /* returns the number of bytes read.
    * 0 for end of file.
    * -1 on error. */
   int64_t (*read)(struct clap_istream *stream, void *buffer, uint64_t size);
};

struct clap_ostream {
   void *ctx;

   /* returns the number of bytes written.
    * -1 on error. */
   int64_t (*write)(struct clap_istream *stream,
                    const void *         buffer,
                    uint64_t             size);
};

#ifdef __cplusplus
}
#endif