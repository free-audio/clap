#pragma once

#include "../../clap.h"
#include "../../stream.h"

static CLAP_CONSTEXPR const char CLAP_EXT_VST3_CONVERT[] = "clap.vst3-convert.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_vst3_convert {
   // Copies the name and VST3 plugin id that we can convert from.
   // [thread-safe]
   void (*get_vst3_plugin_id)(const clap_plugin *plugin, uint8_t *vst3_plugin_uuid);

   // Loads the plugin state from stream using the vst3 chunk.
   // [main-thread]
   bool (*restore_vst3_state)(const clap_plugin *plugin, clap_istream *stream);

   // converts the VST3 param id and normalized value to clap param id and
   // normalized value.
   // [thread-safe]
   bool (*convert_normalized_value)(const clap_plugin *plugin,
                                    uint32_t           vst3_param_id,
                                    double             vst3_normalized_value,
                                    clap_id *          clap_param_id,
                                    double *           clap_plain_value);

   // converts the vst3 param id and plain value to clap param id and
   // plain value.
   // [thread-safe]
   bool (*convert_plain_value)(const clap_plugin *plugin,
                               uint32_t           vst3_param_id,
                               double             vst3_plain_value,
                               clap_id *          clap_param_id,
                               double *           clap_plain_value);
} clap_plugin_vst3_convert;

#ifdef __cplusplus
}
#endif