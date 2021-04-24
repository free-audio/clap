#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_THREAD_CHECK "clap/draft/thread-check"

// This interface is useful to do runtime checks and make
// sure that the functions are called on the correct threads.
struct clap_host_thread_check {
   // Returns true if the "this" thread is the main thread.
   // [thread-safe]
   bool (*is_main_thread)(struct clap_host *host);

   // Returns true if the "this" thread is one of the audio threads.
   // [thread-safe]
   bool (*is_audio_thread)(struct clap_host *host);
};

#ifdef __cplusplus
}
#endif