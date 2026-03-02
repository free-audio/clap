#pragma once

#include "../state.h"

#ifdef __cplusplus
extern "C" {
#endif
  
// This extension allows the plugin to use delayed state restoration while not blocking
// the main thread. The state restoration must be handled in a different thread so the user
// experience is not impacted by the loading time. The plugin shall report its progress to the
// host and notify it when it's done, so the host can update its UI accordingly.
//
// By this the host can disable the play button until all plugins are ready to play, and show one
// or more progress bars if it wants to. This is especially useful for large sample-based plugins
// that need to load a lot of data to restore their state, which can take a significant amount of
// time and impact the user experience if done in the main thread.

static CLAP_CONSTEXPR const char CLAP_EXT_STATE_PROGRESS[] = "clap.state-progress/draft-0";

typedef struct clap_host_state_progress {
   // informs the host that the plugin will restore its state in a different thread.
   // the progress shall be updated in main thread using the progress callback.
   // the host might want to display the progress that is expressed by "steps of total".
   // If the plugin can not determine those steps, it should set steps and total to -1
   // In this case the plugin might not update the host until the restore task is done.
   // 
   // Note that the first call must be during the state->load() call in the main thread.
   // 
   // [thread-safe]
   void(CLAP_ABI *progress)(const clap_host_t *host, int32_t steps, int32_t total);

   // informs the host that the plugin has restored it's state and is ready to play.
   // 
   // [thread-safe]
   void(CLAP_ABI *done)(const clap_host_t *host);
} clap_host_state_progress_t;

#ifdef __cplusplus
}
#endif
