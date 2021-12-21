#pragma once

#include "../clap.h"
#include "../stream.h"
#include "../private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

// This interface provide all the tool to convert a vst3 plugin instance into a clap plugin instance
typedef struct clap_vst3_converter {
   const char *vst3_plugin_id;
   const char *clap_plugin_id;

   // [main-thread]
   bool (*convert_state)(const struct clap_vst3_converter *converter,
                         const clap_istream_t             *vst3,
                         const clap_ostream_t             *clap);

   // converts the vst3 param id and normalized value to clap param id and
   // plain value.
   // [thread-safe]
   bool (*convert_normalized_value)(const struct clap_vst3_converter *converter,
                                    uint32_t                          vst3_param_id,
                                    double                            vst3_normalized_value,
                                    clap_id                          *clap_param_id,
                                    double                           *clap_plain_value);

   // converts the vst3 param id and plain value to clap param id and
   // plain value.
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

#pragma pack(pop)

#ifdef __cplusplus
}
#endif