#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_VST3_CONVERT "clap/draft/vst3-convert"

struct clap_plugin_vst3_convert {
   // Copies the name and VST3 plugin id that we can convert from.
   // Returns the lenght of the name.
   // [thread-safe]
   int32_t (*get_vst3_plugin_id)(struct clap_plugin *plugin,
                                 uint8_t *           vst3_plugin_id);

   // Loads the plugin state from stream using the vst3 chunk.
   // [main-thread]
   bool (*restore_vst3_state)(struct clap_plugin * plugin,
                              struct clap_istream *stream);

   // converts the VST3 param id and normalized value to clap param id and
   // normalized value.
   // [thread-safe]
   bool (*convert_normalize_value)(
      struct clap_plugin *    plugin,
      uint32_t                vst3_param_id,
      double                  vst3_normalized_value,
      uint32_t *              clap_param_id,
      union clap_param_value *clap_normalized_value);

   // converts the vst3 param id and plain value to clap param id and
   // plain value.
   // [thread-safe]
   bool (*convert_plain_value)(struct clap_plugin *    plugin,
                               uint32_t                vst3_param_index,
                               double                  vst3_plain_value,
                               uint32_t *              clap_param_index,
                               union clap_param_value *clap_plain_value);
};

#ifdef __cplusplus
}
#endif