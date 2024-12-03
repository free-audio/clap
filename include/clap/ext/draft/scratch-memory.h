#pragma once

#include "../../plugin.h"

// This extension lets the plugin request "scratch" memory
// from the host. Scratch memory can be accessed during the
// `process()` callback, but its content is not persistent
// between callbacks.
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
// This optimization may allow for reduced memory usage and improved
// CPU cache usage.

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
   // If the plugin calls `reserve()` multiple times, then the
   // final call determines the actual amount of scratch memory
   // that will be available to the plugin. If the final call
   // returns false then no scratch memory will be provided,
   // regardless of any previous calls to `reserve()`.
   //
   // When the plugin is de-activated, the scratch memory
   // is invalidated, and the host may free the memory if
   // appropriate. The plugin will need to reserve scratch
   // memory again the next time it is activated.
   //
   // In the context of plugins and hosts that implement
   // the "thread-pool" extension, scratch memory is assumed
   // to be "thread-local". The plugin should request the maximum
   // amount of scratch memory that it will need on a single
   // thread.
   //
   // The `num_concurrent_buffers` allows the host to ensure it can
   // provide the requested amount of scratch memory when the
   // `thread-pool` extension is used by indicating the maximum number
   // of tasks that the plugin expects to run in parallel, and
   // therefore how many different "thread-local" scratch buffers are
   // required.
   //
   // - If `num_concurrent_buffers` = 0, then thread pool tasks do not
   // require access to scratch memory, and calls to `access()` in a
   // `thread_pool.exec()` context may return NULL.
   // - If `num_concurrent_buffers` = N > 0, then the host must ensure
   // that up to N thread-pool tasks can subsequently call `access()`
   // to receive a thread-local scratch buffer during a
   // `thread_pool.exec()` context.
   //
   // [main-thread & being-activated]
   bool(CLAP_ABI *reserve)(const clap_host_t *host,
                           uint32_t           scratch_size_bytes,
                           uint32_t           num_concurrent_buffers);

   // Asks the host for the previously reserved scratch memory.
   // If the host returned "true" when scratch memory was requested,
   // then this method must return a pointer to a memory block at least
   // as large as the reserved size. If the host returned "false"
   // when scratch memory was requested, then this method must not
   // be called, and will return NULL.
   //
   // If this method is called from the `thread_pool.exec()` callback, then:
   // - if the `num_concurrent_buffers` parameter passed to
   // `reserve()` was zero, then this method may return NULL.
   // - if the `task_index` of the current task is greater than or
   // equal to the value passed to `num_concurrent_buffers`, then this
   // method may return NULL.
   //
   // When called from the `thread_pool.exec()` callback, the returned
   // scratch buffer is "thread-local", that is, no two concurrent
   // executions of this method from different tasks will return the
   // same non-NULL pointer. Non-concurrent executions on different
   // tasks may return the same non-NULL pointer.
   //
   // The buffer returned in the `thread_pool.exec()` callback context
   // might not be the same one as returned in the `process()` context;
   // if a plugin wishes to uses the same scratch buffer everywhere,
   // call `access()` in the `process()` context and share the returned
   // pointer with the thread pool tasks.
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
   // The provided memory is owned by the host, so the plugin must not
   // free the memory.
   //
   // [audio-thread]
   void*(CLAP_ABI *access)(const clap_host_t *host);
} clap_host_scratch_memory_t;

#ifdef __cplusplus
}
#endif
