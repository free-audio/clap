#pragma once

#include "../clap.h"
#include "../stream.h"
#include "../private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

// This interface provide all the tool to convert a vst3 plugin instance into a clap plugin instance
typedef struct clap_clap_converter {
   const char *src_plugin_id;
   const char *dst_plugin_id;

   // [main-thread]
   bool (*convert_state)(const struct clap_clap_converter *converter,
                         const clap_istream_t             *src,
                         const clap_ostream_t             *dst);

   // converts the vst3 param id and plain value to clap param id and
   // plain value.
   // [thread-safe]
   bool (*convert_plain_value)(const struct clap_clap_converter *converter,
                               clap_id                           src_param_id,
                               double                            src_plain_value,
                               clap_id                          *dst_param_id,
                               double                           *dst_plain_value);
} clap_clap_converter_t;

// Factory identifier
static CLAP_CONSTEXPR const char CLAP_CLAP_CONVERTER_FACTORY_ID[] = "clap.clap-converter-factory";

// List all the converters available in the current DSO.
typedef struct clap_clap_converter_factory {
   // Get the number of converters
   uint32_t (*count)(const struct clap_clap_converter_factory *factory);

   // Get the converter at the given index
   const clap_clap_converter_t *(*get)(const struct clap_clap_converter_factory *factory,
                                       uint32_t                                  index);
} clap_clap_converter_factory_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif