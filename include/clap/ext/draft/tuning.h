#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_IDLE "clap/draft/tuning"

// This extension is to providet a dynamic tuning table to the plugin.
struct clap_host_tuning {
   // The plugin can ask the host, the frequency of a given key,
   // at a given time in case the tuning is automated.
   // Returns the frequency in Hz.
   // The plugin is not supposed to query it for each samples,
   // but at a rate that makes sense for low frequency modulations.
   // [audio-thread]
   double (*key_freq)(struct clap_host *  host,
                      struct clap_plugin *plugin,
                      int32_t             key,
                      int32_t             channel,
                      int32_t             frameIndex);
};

#ifdef __cplusplus
}
#endif