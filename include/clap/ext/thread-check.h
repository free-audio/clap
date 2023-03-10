#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_THREAD_CHECK[] = "clap.thread-check";

#ifdef __cplusplus
extern "C" {
#endif

/// @page thread-check
///
/// CLAP defines two symbolic threads:
///
/// main-thread:
///    This is the thread in which most of the interaction between the plugin and host happens.
///    This will be the same OS thread throughout the lifetime of the plug-in.
///    On macOS and Windows, this must be the thread on which gui and timer events are received
///    (i.e., the main thread of the program).
///    It isn't a realtime thread, yet this thread needs to respond fast enough to user interaction,
///    so it is recommended to run long and expensive tasks such as preset indexing or asset loading
///    in dedicated background threads.
///
/// audio-thread:
///    This thread is used for realtime audio processing. Its execution should be as deterministic
///    as possible to meet the audio interface's deadline (can be <1ms). In other words, there is a
///    known set of operations that should be avoided: malloc() and free(), mutexes (spin mutexes
///    are worse), I/O, waiting, ...
///    The audio-thread is something symbolic, there isn't one OS thread that remains the
///    audio-thread for the plugin lifetime. As you may guess, the host is likely to have a
///    thread pool and the plugin.process() call may be scheduled on different OS threads over time.
///    The most important thing is that there can't be two audio-threads at the same time. All the
///    functions marked with [audio-thread] **ARE NOT CONCURRENT**. The host may mark any OS thread,
///    including the main-thread as the audio-thread, as long as it can guarantee that only one OS
///    thread is the audio-thread at a time. The audio-thread can be seen as a concurrency guard for
///    all functions marked with [audio-thread].

// This interface is useful to do runtime checks and make
// sure that the functions are called on the correct threads.
// It is highly recommended that hosts implement this extension.
typedef struct clap_host_thread_check {
   // Returns true if "this" thread is the main thread.
   // [thread-safe]
   bool(CLAP_ABI *is_main_thread)(const clap_host_t *host);

   // Returns true if "this" thread is one of the audio threads.
   // [thread-safe]
   bool(CLAP_ABI *is_audio_thread)(const clap_host_t *host);
} clap_host_thread_check_t;

#ifdef __cplusplus
}
#endif
