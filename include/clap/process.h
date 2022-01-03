#pragma once

#include "events.h"
#include "audio-buffer.h"

#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

enum {
   // Processing failed. The output buffer must be discarded.
   CLAP_PROCESS_ERROR = 0,

   // Processing succeed, keep processing.
   CLAP_PROCESS_CONTINUE = 1,

   // Processing succeed, keep processing if the output is not quiet.
   CLAP_PROCESS_CONTINUE_IF_NOT_QUIET = 2,

   // Processing succeed, but no more processing is required,
   // until next event or variation in audio input.
   CLAP_PROCESS_SLEEP = 3,
};
typedef int32_t clap_process_status;

typedef struct clap_process {
   // A steady sample time counter.
   // This field can be used to calculate the sleep duration between two process calls.
   // This value may be specific to this plugin instance and have no relation to what
   // other plugin instances may receive.
   //
   // Set to -1 if not available, otherwise the value must be greater or equal to 0,
   // and must be increased by at least `frames_count` for the next call to process.
   alignas(8) int64_t steady_time;

   // Number of frame to process
   alignas(4) uint32_t frames_count;

   // time info at sample 0
   // If null, then this is a free running host, no transport events will be provided
   const clap_event_transport_t *transport;

   // Audio buffers, they must have the same count as specified
   // by clap_plugin_audio_ports->get_count().
   // The index maps to clap_plugin_audio_ports->get_info().
   //
   // If a plugin does not implement clap_plugin_audio_ports,
   // then it gets a default stereo input and output.
   const clap_audio_buffer_t *audio_inputs;
   const clap_audio_buffer_t *audio_outputs;
   alignas(4) uint32_t audio_inputs_count;
   alignas(4) uint32_t audio_outputs_count;

   // Input and output events.
   //
   // Events must be sorted by time.
   // The input event list can't be modified.
   //
   // If a plugin does not implement clap_plugin_note_ports,
   // then it gets a default note input and output.
   const clap_input_events_t *in_events;
   const clap_output_events_t *out_events;
} clap_process_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif