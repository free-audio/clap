#pragma once

#include "../../clap.h"

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
} clap_midi_mapping;

typedef struct clap_plugin_midi_mappings {
   // [main-thread]
   uint32_t (*count)(const clap_plugin *plugin);

   // [main-thread]
   bool (*get)(const clap_plugin *plugin, uint32_t index, clap_midi_mapping *mapping);
} clap_plugin_midi_mappings;

typedef struct clap_host_midi_mappings {
   // [main-thread]
   void (*changed)(const clap_host *host);
} clap_host_midi_mappings;

#ifdef __cplusplus
}
#endif