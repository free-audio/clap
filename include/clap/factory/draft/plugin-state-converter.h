#pragma once

#include "../../id.h"
#include "../../plugin-id.h"
#include "../../stream.h"

#ifdef __cplusplus
extern "C" {
#endif

// This interface provides a mechanism for the host to convert a plugin state and its automation
// points to a new plugin.
//
// This is useful to convert from one plugin ABI to another one.
// This is also useful to offer an upgrade path: from EQ version 1 to EQ version 2.
// This can also be used to convert the state of a plugin that isn't maintained anymore into
// another plugin that would be similar.
typedef struct clap_plugin_state_converter {
   const clap_plugin_id_t *src_plugin_id;
   const clap_plugin_id_t *dst_plugin_id;

   // Converts the input state to a state usable by the destination plugin.
   //
   // error_buffer is a place holder of error_buffer_size bytes for storing a null-terminated
   // error message in case of failure, which can be displayed to the user.
   //
   // Returns true on success.
   // [thread-safe]
   bool (*convert_state)(const struct clap_plugin_state_converter *converter,
                         const clap_istream_t                     *src,
                         const clap_ostream_t                     *dst,
                         char                                     *error_buffer,
                         size_t                                    error_buffer_size);

   // Converts a normalized value.
   // Returns true on success.
   // [thread-safe]
   bool (*convert_normalized_value)(const struct clap_plugin_state_converter *converter,
                                    clap_id                                   src_param_id,
                                    double                                    src_normalized_value,
                                    clap_id                                  *dst_param_id,
                                    double                                   *dst_normalized_value);

   // Converts a plain value.
   // Returns true on success.
   // [thread-safe]
   bool (*convert_plain_value)(const struct clap_plugin_state_converter *converter,
                               clap_id                                   src_param_id,
                               double                                    src_plain_value,
                               clap_id                                  *dst_param_id,
                               double                                   *dst_plain_value);
} clap_plugin_state_converter_t;

// Factory identifier
static CLAP_CONSTEXPR const char CLAP_CLAP_CONVERTER_FACTORY_ID[] =
   "clap.plugin-state-converter-factory/1";

// List all the plugin state converters available in the current DSO.
typedef struct clap_plugin_state_converter_factory {
   // Get the number of converters.
   // [thread-safe]
   uint32_t (*count)(const struct clap_plugin_state_converter_factory *factory);

   // Get the converter at the given index.
   // [thread-safe]
   const clap_plugin_state_converter_t *(*get)(
      const struct clap_plugin_state_converter_factory *factory, uint32_t index);
} clap_plugin_state_converter_factory_t;

#ifdef __cplusplus
}
#endif
