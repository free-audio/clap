#pragma once

#include "../../plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @page flush events
///
/// This interface is useful to exchange events with the plugin while not processing.
///
/// This is useful to perform MIDI 2.0 non realtime communication on the main-thread
/// before activating the plugin.

static CLAP_CONSTEXPR const char CLAP_EXT_FLUSH_EVENTS[] = "clap.flush-events/1";

typedef struct clap_plugin_flush_events {
   // Flushes a set of events.
   // This method must not be called concurrently to clap_plugin->process().
   //
   // Note: if the plugin is processing, then the process() call will already perform events I/O, so
   // a call to flush isn't required, also be aware that the plugin may use the sample offset in
   // process(), while this information would be lost within flush().
   //
   // [active ? audio-thread : main-thread]
   void(CLAP_ABI *flush)(const clap_plugin_t        *plugin,
                         const clap_input_events_t  *in,
                         const clap_output_events_t *out);
} clap_plugin_flush_events_t;

typedef struct clap_host_flush_events {
   // Request an event flush.
   //
   // The host will then schedule a call to either:
   // - clap_plugin.process()
   // - clap_plugin_flush_events.flush()
   //
   // This function is always safe to use and should not be called from an [audio-thread] as the
   // plugin would already be within process() or flush().
   //
   // [thread-safe,!audio-thread]
   void(CLAP_ABI *request_flush)(const clap_host_t *host);
} clap_host_flush_events_t;

#ifdef __cplusplus
}
#endif
