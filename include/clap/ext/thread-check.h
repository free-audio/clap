#pragma once

#include "../clap.h"

static CLAP_CONSTEXPR const char CLAP_EXT_THREAD_CHECK[] = "clap.thread-check";

#ifdef __cplusplus
extern "C" {
#endif

// This interface is useful to do runtime checks and make
// sure that the functions are called on the correct threads.
// It is highly recommended to implement this extension
typedef struct clap_host_thread_check {
   // Returns true if "this" thread is the main thread.
   // [thread-safe]
   bool (*is_main_thread)(const clap_host *host);

   // Returns true if "this" thread is one of the audio threads.
   // [thread-safe]
   bool (*is_audio_thread)(const clap_host *host);
} clap_host_thread_check;

#ifdef __cplusplus
}
#endif