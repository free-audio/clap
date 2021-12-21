#pragma once

#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

typedef struct clap_audio_buffer {
   // Either data32 or data64 pointer will be set.
   float ** data32;
   double **data64;
   alignas(4) uint32_t channel_count;
   alignas(4) uint32_t latency;       // latency from/to the audio interface
   alignas(8) uint64_t constant_mask; // mask & (1 << N) to test if channel N is constant
} clap_audio_buffer_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif