#pragma once

#include "private/std.h"
#include "private/macros.h"

/// @page Streams
///
/// ## Notes on using streams
///
/// When working with `clap_istream` and `clap_ostream` objects to load and save
/// state, it is important to keep in mind that the host may limit the number of
/// bytes that can be read or written at a time. The return values for the
/// stream read and write functions indicate how many bytes were actually read
/// or written. You need to use a loop to ensure that you read or write the
/// entirety of your state. Don't forget to also consider the negative return
/// values for the end of file and IO error codes.

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_istream {
   void *ctx; // reserved pointer for the stream

   // returns the number of bytes read; 0 indicates end of file and -1 a read error
   int64_t(CLAP_ABI *read)(const struct clap_istream *stream, void *buffer, uint64_t size);
} clap_istream_t;

typedef struct clap_ostream {
   void *ctx; // reserved pointer for the stream

   // returns the number of bytes written; -1 on write error
   int64_t(CLAP_ABI *write)(const struct clap_ostream *stream, const void *buffer, uint64_t size);
} clap_ostream_t;

#ifdef __cplusplus
}
#endif
