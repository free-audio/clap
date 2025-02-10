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
///    It isn't a realtime thread, yet this thread needs to respond fast enough to allow responsive
///    user interaction, so it is strongly recommended plugins run long,and expensive or blocking
///    tasks such as preset indexing or asset loading in dedicated background threads started by the
///    plugin.
///
/// audio-thread:
///    This thread can be used for realtime audio processing. Its execution should be as
///    deterministic as possible to meet the audio interface's deadline (can be <1ms). There are a
///    known set of operations that should be avoided: malloc() and free(), contended locks and
///    mutexes, I/O, waiting, and so forth.
///
///    The audio-thread is symbolic, there isn't one OS thread that remains the
///    audio-thread for the plugin lifetime. A host is may opt to have a
///    thread pool and the plugin.process() call may be scheduled on different OS threads over time.
///    However, the host must guarantee that single plugin instance will not be two audio-threads
///    at the same time.
///
///    Functions marked with [audio-thread] **ARE NOT CONCURRENT**. The host may mark any OS thread,
///    including the main-thread as the audio-thread, as long as it can guarantee that only one OS
///    thread is the audio-thread at a time in a plugin instance. The audio-thread can be seen as a
///    concurrency guard for all functions marked with [audio-thread].
///
///    The real-time constraint on the [audio-thread] interacts closely with the render extension.
///    If a plugin doesn't implement render, then that plugin must have all [audio-thread] functions
///    meet the real time standard. If the plugin does implement render, and returns true when
///    render mode is set to real-time or if the plugin advertises a hard realtime requirement, it
///    must implement realtime constraints. Hosts also provide functions marked [audio-thread].
///    These can be safely called by a plugin in the audio thread. Therefore hosts must either (1)
///    implement those functions meeting the real-time constraints or (2) not process plugins which
///    advertise a hard realtime constraint or don't implement the render extension. Hosts which
///    provide [audio-thread] functions outside these conditions may experience inconsistent or
///    inaccurate rendering.
///
///  Clap also tags some functions as [thread-safe]. Functions tagged as [thread-safe] can be called
///  from any thread unless explicitly counter-indicated (for instance [thread-safe, !audio-thread])
///  and may be called concurrently.

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
