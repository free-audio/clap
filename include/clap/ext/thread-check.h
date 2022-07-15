#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_THREAD_CHECK[] = "clap.thread-check";

#ifdef __cplusplus
extern "C" {
#endif

/* A note on threads as understood by CLAP:
 *
 * In the [main-thread], a CLAP plugin may carry out allocations, acquire a mutex and do IO,
 * basically anything a reasonably performing program could do. Long running tasks such as
 * indexing presets, should still be run in background threads to keep the [main-thread] responsive.
 *
 * Within an [audio-thread] (of which there may be many in a given host), plugins should strive
 * to meet realtime requirements. I.e. only carry out sufficiently performant and time-bound
 * operations. So mutexes, memory (de-)allocations, IO, UI rendering should generally be avoided.
 *
 * Depending on the host scheduler, the [audio-thread] may move from one OS thread to another,
 * including the same OS thread as the [main-thread].
 * So while plugins are encouraged to use the thread checking functions and may assert
 * is_main_thread() == true or is_audio_thread() == true in a particular context, it should
 * be avoided to assert that the current thread is *NOT* is_main_thread() or is_audio_thread().
 */

// This interface is useful to do runtime checks and make
// sure that the functions are called on the correct threads.
// It is highly recommended that hosts implement this extension.
typedef struct clap_host_thread_check {
   // Returns true if "this" thread is the main thread.
   // [thread-safe]
   bool (*is_main_thread)(const clap_host_t *host);

   // Returns true if "this" thread is one of the audio threads.
   // [thread-safe]
   bool (*is_audio_thread)(const clap_host_t *host);
} clap_host_thread_check_t;

#ifdef __cplusplus
}
#endif
