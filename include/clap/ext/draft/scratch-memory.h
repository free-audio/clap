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
   // The plugin may call this method multiple times (for
   // example, gradually decreasing the amount of scratch
   // being asked for until the host returns true), however,
   // the plugin should avoid calling this method un-neccesarily
   // since the host implementation may be relatively expensive.
   // If the plugin calls this method multiple times, then the
   // final call determines the actual amount of scratch memory
   // that will be available to the plugin.
   //
   // Note that any memory the host allocates to satisfy
   // the requested scratch size can be de-allocated by the
   // host when the plugin is de-activated.
   //
   // In the context of plugins and hosts that implement
   // the "thread-pool" extension, scratch memory is assumed
   // to be "thread-local". The plugin should request the maximum
   // amount of scratch memory that it will need on a single
   // thread. Accordingly, the host must ensure that each
   // thread can independently provide the requested amount
   // of scratch memory.
   //
   // [main-thread & being-activated]
   bool(CLAP_ABI *reserve)(const clap_host_t *host, uint32_t scratch_size_bytes);

   // Asks the host for the previously reserved scratch memory.
   // If the host returned "true" when scratch memory was requested,
   // then this method must return a pointer to a memory block at least
   // as large as the reserved size. If the host returned "false"
   // when scratch memory was requested, then this method must not
   // be called.
   //
   // This method may only be called by the plugin from the audio thread,
   // (i.e. during the process() or thread_pool.exec() callback), and
   // the provided memory is only valid until the plugin returns from
   // that callback. The plugin must not hold any references to data
   // that lives in the scratch memory after returning from the callback,
   // as that data will likely be over-written by another plugin using
   // the same scratch memory.
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
