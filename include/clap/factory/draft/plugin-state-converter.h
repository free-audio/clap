#pragma once

#include "../../id.h"
#include "../../universal-plugin-id.h"
#include "../../stream.h"
#include "../../version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_state_converter_descriptor {
   clap_version_t clap_version;

   clap_universal_plugin_id_t src_plugin_id;
   clap_universal_plugin_id_t dst_plugin_id;

   const char *id;          // eg: "com.u-he.diva-converter", mandatory
   const char *name;        // eg: "Diva Converter", mandatory
   const char *vendor;      // eg: "u-he"
   const char *version;     // eg: 1.1.5
   const char *description; // eg: "Official state converter for u-he Diva."
} clap_plugin_state_converter_descriptor_t;

// This interface provides a mechanism for the host to convert a plugin state and its automation
// points to a new plugin.
//
// This is useful to convert from one plugin ABI to another one.
// This is also useful to offer an upgrade path: from EQ version 1 to EQ version 2.
// This can also be used to convert the state of a plugin that isn't maintained anymore into
// another plugin that would be similar.
typedef struct clap_plugin_state_converter {
   const clap_plugin_state_converter_descriptor_t *desc;

   void *converter_data;

   // Destroy the converter.
   void (*destroy)(struct clap_plugin_state_converter *converter);

   // Converts the input state to a state usable by the destination plugin.
   //
   // error_buffer is a place holder of error_buffer_size bytes for storing a null-terminated
   // error message in case of failure, which can be displayed to the user.
   //
   // Returns true on success.
   // [thread-safe]
   bool (*convert_state)(struct clap_plugin_state_converter *converter,
                         const clap_istream_t               *src,
                         const clap_ostream_t               *dst,
                         char                               *error_buffer,
                         size_t                              error_buffer_size);

   // Converts a normalized value.
   // Returns true on success.
   // [thread-safe]
   bool (*convert_normalized_value)(struct clap_plugin_state_converter *converter,
                                    clap_id                             src_param_id,
                                    double                              src_normalized_value,
                                    clap_id                            *dst_param_id,
                                    double                             *dst_normalized_value);

   // Converts a plain value.
   // Returns true on success.
   // [thread-safe]
   bool (*convert_plain_value)(struct clap_plugin_state_converter *converter,
                               clap_id                             src_param_id,
                               double                              src_plain_value,
                               clap_id                            *dst_param_id,
                               double                             *dst_plain_value);
} clap_plugin_state_converter_t;

// Factory identifier
static CLAP_CONSTEXPR const char CLAP_PLUGIN_STATE_CONVERTER_FACTORY_ID[] =
   "clap.plugin-state-converter-factory/1";

// List all the plugin state converters available in the current DSO.
typedef struct clap_plugin_state_converter_factory {
   // Get the number of converters.
   // [thread-safe]
   uint32_t (*count)(const struct clap_plugin_state_converter_factory *factory);

   // Retrieves a plugin state converter descriptor by its index.
   // Returns null in case of error.
   // The descriptor must not be freed.
   // [thread-safe]
   const clap_plugin_state_converter_descriptor_t *(*get_descriptor)(
      const struct clap_plugin_state_converter_factory *factory, uint32_t index);

   // Create a plugin state converter by its converter_id.
   // The returned pointer must be freed by calling converter->destroy(converter);
   // Returns null in case of error.
   // [thread-safe]
   clap_plugin_state_converter_t *(*create)(
      const struct clap_plugin_state_converter_factory *factory, const char *converter_id);
} clap_plugin_state_converter_factory_t;

#ifdef __cplusplus
}
#endif
