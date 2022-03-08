#pragma once

#include "../../plugin.h"
#include "../../events.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TUNING[] = "clap.tuning.draft/1";

#ifdef __cplusplus
extern "C" {
#endif

// Use clap_host_event_registry(host, CLAP_EXT_TUNING, &space_id) to know the event space.
//
// Defines the tuning to be used on the given port/channel.
typedef struct clap_event_tuning {
   clap_event_header_t header;

   int16_t port_index; // -1 global
   int16_t channel;    // 0..15, -1 global
   clap_id tunning_id;
} clap_event_tuning_t;

typedef struct clap_tuning_info {
   char name[CLAP_NAME_SIZE];
   bool is_dynamic; // true if the values may vary with time

   // Table of relative tuning values in semi-tones against equal temperament with A4=440Hz
   double table[128];
} clap_tuning_info_t;

// This extension provides a dynamic tuning table to the plugin.
typedef struct clap_host_tuning {
   // Gets the relative tuning in semitone against equal temperament with A4=440Hz.
   // The plugin may query the tuning at a rate that makes sense for *low* frequency modulations.
   //
   // If the tuning_id is not found, then it shall gracefuly return a sensible value.
   //
   // [audio-thread]
   double (*get_relative)(const clap_host_t *host, clap_id tuning_id, int32_t key, int32_t channel);

   // Gets info about a tuning
   // [main-thread]
   bool (*get_info)(const clap_host_t *host, clap_id tuning_id, clap_tuning_info_t *info);
} clap_host_tuning_t;

#ifdef __cplusplus
}
#endif
