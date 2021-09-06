#pragma once

#include "../../clap.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TUNING[] = "clap.tuning.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

// This extension provides a dynamic tuning table to the plugin.
typedef struct clap_host_tuning {
   // The plugin can ask the host, the frequency of a given key,
   // at a given time in case the tuning is automated.
   // Returns the frequency in Hz.
   // The plugin is not supposed to query it for each samples,
   // but at a rate that makes sense for low frequency modulations.
   // [audio-thread]
   double (*get)(const clap_host *host, int32_t key, int32_t channel);
} clap_host_tuning;

#ifdef __cplusplus
}
#endif