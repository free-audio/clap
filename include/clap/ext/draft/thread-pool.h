#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

/// @page
///
/// This extension let the plugin use the host's thread pool
///
/// The plugin must provide @ref clap_plugin_thread_pool, and the host may provide @ref
/// clap_host_thread_pool. If it doesn't, the plugin should process its data by its own mean. In the
/// worst case, a single threaded for-loop.
///
/// Simple example with N voices to process
///
/// @code
/// void myplug_thread_pool_exec(clap_plugin *plugin, uint32_t voice_index)
/// {
///    compute_voice(plugin, voice_index);
/// }
///
/// void myplud_process(clap_plugin *plugin, const clap_process *process)
/// {
///    ...
///    if (host_thread_pool && host_thread_pool.exec)
///       host_thread_pool.exec(host, plugin, N);
///    else
///       for (uint32_t i = 0; i < N; ++i)
///          myplug_thread_pool_exec(plugin, N);
///    ...
/// }
/// @endcode

#define CLAP_EXT_THREAD_POOL "clap/draft/thread-pool"

typedef struct clap_plugin_thread_pool {
   // Called by the thread pool
   void (*exec)(clap_plugin *plugin, uint32_t task_index);
} clap_plugin_thread_pool;

typedef struct clap_host_thread_pool {
   // Schedule num_tasks jobs in the host thread pool.
   // It can't be called concurrently or from the thread pool.
   // Will block until all the tasks are processed.
   void (*exec)(clap_host *host, clap_plugin *plugin, uint32_t num_tasks);
} clap_host_thread_pool;

#ifdef __cplusplus
}
#endif