#pragma once

#include "../../plugin.h"

// This extension lets the plugin request "scratch" memory
// from the host. Scratch memory can be accessed during the
// `process()` callback, but is not persistent between callbacks.
//
// The motivation for this extension is to allow the plugin host
// to "share" a single scratch buffer across multiple plugin
// instances.
//
// For example, imagine the host needs to process three plugins
// in sequence, and each plugin requires 10K of scratch memory.
// If each plugin pre-allocates its own scratch memory, then 30K
// of memory is being allocated in total. However, if each plugin
// requests 10K of scratch memory from the host, then the host can
// allocate a single 10K scratch buffer, and make it available to all
// three plugins.
//
// On memory-constrained platforms, this optimization may allow for
// more plugins to be used simultaneously. On platforms with lots
// of memory, this optimization may improve CPU cache usage.

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
   // the requested scratch size can be de-allocated by the
   // host when the plugin is de-activated.
   //
   // [main-thread & being-activated]
   bool(CLAP_ABI *pre_reserve)(const clap_host_t *host, size_t scratch_size_bytes);

   // Asks the host for the previously requested scratch memory.
   // If the host returned "true" when scratch memory was requested,
   // then this method must return a pointer to a memory block at least
   // as large as the requested size. If the host returned "false"
   // when scratch memory was requested then this method should return
   // null.
   //
   // The provided memory is not initialized, and may have been used
   // by other plugin instances, so the plugin must correctly initialize
   // the memory when using it.
   //
   // The provided memory is owned by the host, so the plugin should not
   // attempt to free the memory.
   //
   // [audio-thread]
   void*(CLAP_ABI *access)(const clap_host_t *host);
} clap_host_scratch_memory_t;

#ifdef __cplusplus
}
#endif
