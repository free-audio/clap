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

typedef struct clap_plugin_state_convert_param_value {
   clap_id param_id;
   double  value;
} clap_plugin_state_converter_param_value_t;

enum clap_plugin_state_convert_flags {
   // When this flag is set, then we are converting an automation point.
   CLAP_PLUGIN_STATE_CONVERTER_FOR_AUTOMATION = 1 << 0,

   // When this flag is set, then we are converting a modulation mapping.
   // For example we can ask the target value for 0.5 and then 0.6 and
   // calculate the new modulation mapping range.
   CLAP_PLUGIN_STATE_CONVERTER_FOR_MODULATION_MAPPING = 1 << 1,

   // When set, input and output values must be normalized.
   CLAP_PLUGIN_STATE_CONVERTER_VALUE_IS_NORMALIZED = 1 << 2,
};

// This interface provides a mechanism for the host to convert a plugin state and its automation
// points to a new plugin.
//
// This is useful to convert from one plugin ABI to another one.
// This is also useful to offer an upgrade path: from EQ version 1 to EQ version 2.
// This can also be used to convert the state of a plugin that isn't maintained anymore into
// another plugin that would be similar.
//
// Note:
// We recommend to first convert the state and then the automation points.
// Because the points convertion may depend on the plugin state, and the converter is
// stateful so it needs to know the state in order to achieve optimal automation conversion.
// The converter may also assume that every call to convert_value where
// CLAP_PLUGIN_STATE_CONVERTER_FOR_AUTOMATION is set happens in chronological order.
//
// Note:
// Sometimes, there's a many to many relationship between source parameters and target
// parameters. For example in plugin A, you have two parameters Phase, Amplitude which describes
// some polar coordinates. In plugin B, you have X, Y parameters which are euclidian coordinates.
// It is not possible to perfectly convert from A to B or B to A, because of the automation
// curves, ... Though, for a best effort approach we can support one parameter that translates to
// many. For example, in A you have a 3 state enum parameter, that is translated into two bool
// parameters in plugin B, or you have a parameter that has been duplicated in B.
//
// That is why, when we convert a parameter value, it can produce 0 to many target parameter
// value.
typedef struct clap_plugin_state_converter {
   const clap_plugin_state_converter_descriptor_t *desc;

   void *converter_data;

   // Destroy the converter.
   void(CLAP_ABI *destroy)(struct clap_plugin_state_converter *converter);

   // Converts the input state to a state usable by the destination plugin.
   //
   // error_buffer is a place holder of error_buffer_size bytes for storing a null-terminated
   // error message in case of failure, which can be displayed to the user.
   bool(CLAP_ABI *convert_state)(struct clap_plugin_state_converter *converter,
                                 const clap_istream_t               *src,
                                 const clap_ostream_t               *dst,
                                 char                               *error_buffer,
                                 size_t                              error_buffer_size);

   // Get the parameter mapping.
   //
   // Note: one source parameter can map to many destination parameters, see the note above.
   int32_t(CLAP_ABI *get_mapping)(struct clap_plugin_state_converter *converter,
                                  clap_id                             src_param_id,
                                  clap_id                            *dst_param_ids,
                                  uint32_t                            dst_param_ids_size);

   // Converts a value.
   // flags is a bitmask, see clap_plugin_state_convert_flags
   // src is a single parameter value.
   // dsts is an array of parameter values.
   // dsts_size is the size of the dsts array.
   // See get_max_target_parameters() and the struct's note.
   //
   // Returns the number of converted parameters, or -1 on error.
   //
   // Note:
   // convert_value assumes the state of the plugin to the one from the most recent call to
   // convert_state.
   int32_t(CLAP_ABI *convert_value)(struct clap_plugin_state_converter        *converter,
                                    uint32_t                                   flags,
                                    clap_plugin_state_converter_param_value_t *src,
                                    clap_plugin_state_converter_param_value_t *dsts,
                                    uint32_t                                   dsts_size);
} clap_plugin_state_converter_t;

// Factory identifier
static CLAP_CONSTEXPR const char CLAP_PLUGIN_STATE_CONVERTER_FACTORY_ID[] =
   "clap.plugin-state-converter-factory/2";

// List all the plugin state converters available in the current DSO.
typedef struct clap_plugin_state_converter_factory {
   // Get the number of converters.
   // [thread-safe]
   uint32_t(CLAP_ABI *count)(const struct clap_plugin_state_converter_factory *factory);

   // Retrieves a plugin state converter descriptor by its index.
   // Returns null in case of error.
   // The descriptor must not be freed.
   // [thread-safe]
   const clap_plugin_state_converter_descriptor_t *(CLAP_ABI *get_descriptor)(
      const struct clap_plugin_state_converter_factory *factory, uint32_t index);

   // Create a plugin state converter by its converter_id.
   // The returned pointer must be freed by calling converter->destroy(converter);
   //
   // Returns a stateful converter on success, or null on error.
   //
   // Note: the returned converter isn't thread-safe.
   //
   // [thread-safe]
   clap_plugin_state_converter_t *(CLAP_ABI *create)(
      const struct clap_plugin_state_converter_factory *factory, const char *converter_id);
} clap_plugin_state_converter_factory_t;

#ifdef __cplusplus
}
#endif
