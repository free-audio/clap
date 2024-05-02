#pragma once

#include "../../plugin.h"
#include "../../events.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TUNING[] = "clap.tuning/2";

#ifdef __cplusplus
extern "C" {
#endif

// Use clap_host_event_registry->query(host, CLAP_EXT_TUNING, &space_id) to know the event space.
//
// This event defines the tuning to be used on the given port/channel.
typedef struct clap_event_tuning {
   clap_event_header_t header;

   int16_t port_index; // -1 global
   int16_t channel;    // 0..15, -1 global
   clap_id tunning_id;
} clap_event_tuning_t;

typedef struct clap_tuning_info {
   clap_id tuning_id;
   char    name[CLAP_NAME_SIZE];
   bool    is_dynamic; // true if the values may vary with time
} clap_tuning_info_t;

typedef struct clap_plugin_tuning {
   // Called when a tuning is added or removed from the pool.
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_plugin_t *plugin);
} clap_plugin_tuning_t;

// This extension provides a dynamic tuning table to the plugin.
typedef struct clap_host_tuning {
   // Gets the relative tuning in semitones against equal temperament with A4=440Hz.
   // The plugin may query the tuning at a rate that makes sense for *low* frequency modulations.
   //
   // If the tuning_id is not found or equals to CLAP_INVALID_ID,
   // then the function shall gracefully return a sensible value.
   //
   // sample_offset is the sample offset from the beginning of the current process block.
   //
   // should_play(...) should be checked before calling this function.
   //
   // [audio-thread & in-process]
   double(CLAP_ABI *get_relative)(const clap_host_t *host,
                                  clap_id            tuning_id,
                                  int32_t            channel,
                                  int32_t            key,
                                  uint32_t           sample_offset);

   // Returns true if the note should be played.
   // [audio-thread & in-process]
   bool(CLAP_ABI *should_play)(const clap_host_t *host,
                               clap_id            tuning_id,
                               int32_t            channel,
                               int32_t            key);

   // Returns the number of tunings in the pool.
   // [main-thread]
   uint32_t(CLAP_ABI *get_tuning_count)(const clap_host_t *host);

   // Gets info about a tuning
   // Returns true on success and stores the result into info.
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_host_t  *host,
                            uint32_t            tuning_index,
                            clap_tuning_info_t *info);
} clap_host_tuning_t;

#ifdef __cplusplus
}
#endif
