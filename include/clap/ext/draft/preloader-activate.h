#pragma once

#include "../../preloader.h"

static CLAP_CONSTEXPR const char CLAP_EXT_PRELOADER_ACTIVATE[] = "clap.preloader_activate/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_preloader_activate {
   /// Prepares plugin activation for a given audio configuration.
   ///
   /// This may e.g. pre-allocate buffers of the appropriate size and configuration, which will
   /// be transferred to the plugin on Commit, at which point they can be directly used in
   /// a subsequent called to `clap_plugin.activate`, instead of having to be allocated on the main
   /// thread.
   ///
   /// Note that this does *not* activate the plugin on Commit. It only transfers the pre-allocated
   /// resources to the plugin for a subsequent `clap_plugin.activate` call.
   ///
   /// Returns `true` if this succeeded, or `false` if this failed.
   ///
   /// A `prepare_activate` failure is not fatal, it only means the plugin may have to perform
   /// more work on the main thread during the `clap_plugin.activate` call.
   // [preloader-thread]
   bool(CLAP_ABI *prepare_activate)(const clap_preloader_t *preloader,
                                    double                  sample_rate,
                                    uint32_t                min_frames_count,
                                    uint32_t                max_frames_count);
} clap_preloader_activate_t;

#ifdef __cplusplus
}
#endif
