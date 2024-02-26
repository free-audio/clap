#pragma once

#include "../../plugin.h"

// This extension lets a plugin expose its MIDI controls implementation.
// The primary goal is to allow a host to create a MIDI-CI Property Exchange "ChCtrlList" resource for the plugin,
// which is presented to a keyboard / controller.
// A keyboard can then assign its knobs to suitable controllers, decide between sending poly or channel afertouch etc.
// Also, hosts can use this extension to indicate to users which controls actually work, and display proper names.

// TODO : clap_midi_control_info could/should contain more (optional) info (min/max, stepcount, paramPath etc.)

static CLAP_CONSTEXPR const char CLAP_EXT_MIDI_CONTROLS[] = "clap.midi-controls.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // MIDI voice channel message status
   CLAP_MIDI_CONTROL_STATUS_REGISTERED_PER_NOTE_CONTROLLER = 0x00,
   CLAP_MIDI_CONTROL_STATUS_ASSIGNABLE_PER_NOTE_CONTROLLER = 0x10,
   CLAP_MIDI_CONTROL_STATUS_RPN = 0x20,
   CLAP_MIDI_CONTROL_STATUS_NRPN = 0x30,
   CLAP_MIDI_CONTROL_STATUS_PER_NOTE_PITCHBEND = 0x60,
   CLAP_MIDI_CONTROL_STATUS_POLY_AFTERTOUCH = 0xA0,
   CLAP_MIDI_CONTROL_STATUS_CONTROLLER = 0xB0,
   CLAP_MIDI_CONTROL_STATUS_CHANNEL_AFTERTOUCH = 0xD0,
   CLAP_MIDI_CONTROL_STATUS_PITCHBEND = 0xE0,
};

/* This describes a MIDI control/message */
typedef struct clap_midi_control_info {
   // CLAP_MIDI_CONTROL_STATUS, describes what kind of control this struct is about.
   uint32_t status;

   // controller number, nrpn number etc.
   // zero if not applicable the status value
   uint32_t number;

   // Priority of this control (from a performing musician's point of view)
   // 1..5, a value of 1 is the most important, descending down to 5 as the least important.
   // Multiple controls can have the same priority. In this case the control at the lowest index
   // (see clap_plugin_midi_controls.get_info) is more important than the next ones.
   uint32_t priority;		

   // Display name
   // If name is empty the host can assume default MIDI controller names (cc7 is Volume etc.)
   char name[CLAP_NAME_SIZE];
} clap_midi_control_info_t;

typedef struct clap_plugin_midi_controls {
   // Returns the number of clap_midi_control_info structs for a channel.
   // channel is 0..15
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin,
                             int16_t              port_index,
                             uint16_t             channel);

   // Fills midi_control_info. Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_plugin_t *plugin,
                            int16_t              port_index,
                            uint16_t             channel,
                            uint32_t             index,
                            clap_midi_control_info_t   *midi_control_info);

   //if false all channels are the same, so the host doesn't have to scan them all.
   // [main-thread]
   bool(CLAP_ABI *is_multitimbral)(const clap_plugin_t *plugin,
                                   int16_t              port_index);
} clap_plugin_midi_controls_t;

typedef struct clap_host_midi_controls {
   // Rescan the full list of structs.
   // This can happen if an instrument switches to a different patch, for example.
   // port_index = -1 means 'all ports'
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host,
                           int16_t            port_index);
} clap_host_midi_controls_t;

#ifdef __cplusplus
}
#endif