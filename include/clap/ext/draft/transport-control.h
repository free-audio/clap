#pragma once

#include "../../plugin.h"

// This extension lets the plugin submit transport requests to the host.
// The host has no obligation to execute these requests, so the interface may be
// partially working.

static CLAP_CONSTEXPR const char CLAP_EXT_TRANSPORT_CONTROL[] = "clap.transport-control/1";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_host_transport_control {
   // Jumps back to the start point and starts the transport
   // [main-thread]
   void(CLAP_ABI *request_start)(const clap_host_t *host);

   // Stops the transport, and jumps to the start point
   // [main-thread]
   void(CLAP_ABI *request_stop)(const clap_host_t *host);

   // If not playing, starts the transport from its current position
   // [main-thread]
   void(CLAP_ABI *request_continue)(const clap_host_t *host);

   // If playing, stops the transport at the current position
   // [main-thread]
   void(CLAP_ABI *request_pause)(const clap_host_t *host);

   // Equivalent to what "space bar" does with most DAWs
   // [main-thread]
   void(CLAP_ABI *request_toggle_play)(const clap_host_t *host);

   // Jumps the transport to the given position.
   // Does not start the transport.
   // [main-thread]
   void(CLAP_ABI *request_jump)(const clap_host_t *host, clap_beattime position);

   // Sets the loop region
   // [main-thread]
   void(CLAP_ABI *request_loop_region)(const clap_host_t *host,
                                       clap_beattime      start,
                                       clap_beattime      duration);

   // Toggles looping
   // [main-thread]
   void(CLAP_ABI *request_toggle_loop)(const clap_host_t *host);

   // Enables/Disables looping
   // [main-thread]
   void(CLAP_ABI *request_enable_loop)(const clap_host_t *host, bool is_enabled);

   // Enables/Disables recording
   // [main-thread]
   void(CLAP_ABI *request_record)(const clap_host_t *host, bool is_recording);

   // Toggles recording
   // [main-thread]
   void(CLAP_ABI *request_toggle_record)(const clap_host_t *host);
} clap_host_transport_control_t;

#ifdef __cplusplus
}
#endif
