#pragma once

#include "events.h"
#include "audio-buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // Processing failed. The output buffer must be discarded.
   CLAP_PROCESS_ERROR = 0,

   // Processing succeeded, keep processing.
   CLAP_PROCESS_CONTINUE = 1,

   // Processing succeeded, keep processing if the output is not quiet.
   CLAP_PROCESS_CONTINUE_IF_NOT_QUIET = 2,

   // Rely upon the plugin's tail to determine if the plugin should continue to process.
   // see clap_plugin_tail
   CLAP_PROCESS_TAIL = 3,

   // Processing succeeded, but no more processing is required,
   // until the next event or variation in audio input.
   CLAP_PROCESS_SLEEP = 4,
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
   int64_t steady_time;

   // Number of frames to process
   uint32_t frames_count;

   // time info at sample 0
   // If null, then this is a free running host, no transport events will be provided
   const clap_event_transport_t *transport;

   // Audio buffers, they must have the same count as specified
   // by clap_plugin_audio_ports->count().
   // The index maps to clap_plugin_audio_ports->get().
   // Input buffer and its contents are read-only.
   const clap_audio_buffer_t *audio_inputs;
   clap_audio_buffer_t       *audio_outputs;
   uint32_t                   audio_inputs_count;
   uint32_t                   audio_outputs_count;

   // The input event list can't be modified.
   // Input read-only event list. The host will deliver these sorted in sample order.
   const clap_input_events_t  *in_events;

   // Output event list. The plugin must insert events in sample sorted order when inserting events
   const clap_output_events_t *out_events;
} clap_process_t;

#ifdef __cplusplus
}
#endif
