#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_audio_buffer {
   // Either data32 or data64 pointer will be set.
   float ** data32;
   double **data64;
   uint32_t channel_count;
   uint32_t latency;       // latency from/to the audio interface
   uint64_t constant_mask; // mask & (1 << N) to test if channel N is constant
} clap_audio_buffer;

#ifdef __cplusplus
}
#endif