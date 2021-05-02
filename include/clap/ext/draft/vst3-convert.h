#pragma once

#include "../../clap.h"
#include "../../stream.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_VST3_CONVERT "clap/draft/vst3-convert"

typedef struct clap_plugin_vst3_convert {
   // Copies the name and VST3 plugin id that we can convert from.
   // Returns the lenght of the name.
   // [thread-safe]
   int32_t (*get_vst3_plugin_id)(clap_plugin *plugin, uint8_t *vst3_plugin_id);

   // Loads the plugin state from stream using the vst3 chunk.
   // [main-thread]
   bool (*restore_vst3_state)(clap_plugin *plugin, clap_istream *stream);

   // converts the VST3 param id and normalized value to clap param id and
   // normalized value.
   // [thread-safe]
   bool (*convert_normalize_value)(clap_plugin *     plugin,
                                   uint32_t          vst3_param_id,
                                   double            vst3_normalized_value,
                                   int32_t *         clap_param_id,
                                   clap_param_value *clap_normalized_value);

   // converts the vst3 param id and plain value to clap param id and
   // plain value.
   // [thread-safe]
   bool (*convert_plain_value)(clap_plugin *     plugin,
                               uint32_t          vst3_param_id,
                               double            vst3_plain_value,
                               int32_t *         clap_param_id,
                               clap_param_value *clap_plain_value);
} clap_plugin_vst3_convert;

#ifdef __cplusplus
}
#endif