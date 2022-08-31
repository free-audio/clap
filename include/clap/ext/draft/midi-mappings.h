#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_MIDI_MAPPINGS[] = "clap.midi-mappings.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_MIDI_MAPPING_CC7,
   CLAP_MIDI_MAPPING_CC14,
   CLAP_MIDI_MAPPING_RPN,
   CLAP_MIDI_MAPPING_NRPN,
};
typedef int32_t clap_midi_mapping_type;

typedef struct clap_midi_mapping {
   int32_t channel;
   int32_t number;
   clap_id param_id;
} clap_midi_mapping_t;

typedef struct clap_plugin_midi_mappings {
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t *plugin, uint32_t index, clap_midi_mapping_t *mapping);
} clap_plugin_midi_mappings_t;

typedef struct clap_host_midi_mappings {
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_midi_mappings_t;

#ifdef __cplusplus
}
#endif
