#pragma once

#include "../../clap.h"
#include "../../stream.h"

static CLAP_CONSTEXPR const char CLAP_EXT_VST2_CONVERT[] = "clap.vst2-convert.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_vst2_convert {
   // Copies the name and VST2 plugin id that we can convert from.
   // Returns the lenght of the name.
   // [thread-safe]
   int32_t (*get_vst2_plugin_id)(const clap_plugin *plugin,
                                 uint32_t *         vst2_plugin_id,
                                 char *             name,
                                 uint32_t           name_size);

   // Loads the plugin state from stream using the VST2 chunk.
   // [main-thread]
   bool (*restore_vst2_state)(const clap_plugin *plugin, clap_istream *stream);

   // converts the vst2 param id and normalized value to clap param id and
   // plain value.
   // [thread-safe]
   bool (*convert_normalized_value)(const clap_plugin *plugin,
                                    uint32_t           vst2_param_id,
                                    double             vst2_normalized_value,
                                    clap_id *          clap_param_id,
                                    double *           clap_plain_value);

   // converts the vst2 param id and plain value to clap param id and
   // plain value.
   // [thread-safe]
   bool (*convert_plain_value)(const clap_plugin *plugin,
                               uint32_t           vst2_param_id,
                               double             vst2_plain_value,
                               clap_id *          clap_param_id,
                               double *           clap_plain_value);
} clap_plugin_vst2_convert;

#ifdef __cplusplus
}
#endif