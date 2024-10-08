#pragma once

#include "../../plugin.h"

// This extension lets the plugin request "scratch" memory
// from the host. Scratch memory can be accessed during the
// `process()` callback, but is not persistent between callbacks.

static CLAP_CONSTEXPR const char CLAP_EXT_SCRATCH_MEMORY[] = "clap.scratch-memory/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host_scratch_memory {
   // Asks the host for certain amount of scratch memory.
   // If the host is unable to provide the memory, it should
   // return "false".
   //
   // Note that any memory the host allocates to satisfy
   // the requested scratch size can be de-allocated
   // when the plugin is de-activated.
   //
   // [main-thread & being-activated]
   bool(CLAP_ABI *request_size)(const clap_host_t *host, size_t scratch_size_bytes);

   // Asks the host for the previously requested scratch memory.
   // If the host returned "true" when scratch memory was requested,
   // then this method must return a pointer to a memory block at least
   // as large as the requested size. If the host returned "false"
   // when scratch memory was requested then this method should return
   // null.
   //
   // Note that this memory may be uninitialized, so the plugin may
   // want to "zero" the memory before using it.
   //
   // [audio-thread]
   void*(CLAP_ABI *access)(const clap_host_t *host);
};

#ifdef __cplusplus
}
#endif
