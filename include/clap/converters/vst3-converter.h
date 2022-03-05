#pragma once

#include "../clap.h"
#include "../stream.h"

#ifdef __cplusplus
extern "C" {
#endif

// This interface provide all the tool to convert a vst3 plugin instance into a clap plugin instance
typedef struct clap_vst3_converter {
   // The VST FUID can be constructed by:
   // Steinberg::FUID::fromTUID(conv->vst3_plugin_tuid);
   const int8_t vst3_plugin_tuid[16];
   const char  *clap_plugin_id;

   // [main-thread]
   bool (*convert_state)(const struct clap_vst3_converter *converter,
                         const clap_istream_t             *vst3_processor,
                         const clap_istream_t             *vst3_editor,
                         const clap_ostream_t             *clap);

   // converts the vst3 param id and normalized value to clap.
   // [thread-safe]
   bool (*convert_normalized_value)(const struct clap_vst3_converter *converter,
                                    uint32_t                          vst3_param_id,
                                    double                            vst3_normalized_value,
                                    clap_id                          *clap_param_id,
                                    double                           *clap_normalized_value);

   // converts the vst3 param id and plain value to clap.
   // [thread-safe]
   bool (*convert_plain_value)(const struct clap_vst3_converter *converter,
                               uint32_t                          vst3_param_id,
                               double                            vst3_plain_value,
                               clap_id                          *clap_param_id,
                               double                           *clap_plain_value);
} clap_vst3_converter_t;

// Factory identifier
static CLAP_CONSTEXPR const char CLAP_VST3_CONVERTER_FACTORY_ID[] = "clap.vst3-converter-factory";

// List all the converters available in the current DSO.
typedef struct clap_vst3_converter_factory {
   // Get the number of converters
   uint32_t (*count)(const struct clap_vst3_converter_factory *factory);

   // Get the converter at the given index
   const clap_vst3_converter_t *(*get)(const struct clap_vst3_converter_factory *factory,
                                       uint32_t                                  index);
} clap_vst3_converter_factory_t;

#ifdef __cplusplus
}
#endif
