#pragma once

#include "events.h"
#include "audio-buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

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
   uint64_t steady_time;  // a steady sample time counter, requiered
   uint32_t frames_count; // number of frame to process

   // time info at sample 0
   // If null, then this is a free running host, no transport events will be provided
   const clap_event_transport *transport;

   // Audio buffers, they must have the same count as specified
   // by clap_plugin_audio_ports->get_count().
   // The index maps to clap_plugin_audio_ports->get_info().
   //
   // If a plugin does not implement clap_plugin_audio_ports,
   // then it gets a default stereo input and output.
   const clap_audio_buffer *audio_inputs;
   const clap_audio_buffer *audio_outputs;
   uint32_t                 audio_inputs_count;
   uint32_t                 audio_outputs_count;

   /* events */
   const clap_event_list *in_events;
   const clap_event_list *out_events;
} clap_process;

#ifdef __cplusplus
}
#endif