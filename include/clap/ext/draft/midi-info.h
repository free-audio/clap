#pragma once

#include "../../plugin.h"

// This extension lets a plugin expose it's MIDI implementation.
// The primary goal is to allow a host to create a MIDI-CI Property Exchange "ChCtrlList" resource for the plugin,
// so keyboard controllers can assign their knobs to suitable controllers.

//TODO : clap_midiinfo could contain more (optional) info (min/max, stepcount, paramPath etc.)
//TODO : for completeness this should work per-input port. Is that worth it?

static CLAP_CONSTEXPR const char CLAP_EXT_MIDIINFO[] = "clap.midi-info.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   //MIDI 2.0 message status
   CLAP_MIDIINFO_STATUS_REGISTERED_PER_NOTE_CONTROLLER = 0x00,
   CLAP_MIDIINFO_STATUS_ASSIGNABLE_PER_NOTE_CONTROLLER = 0x10,
   CLAP_MIDIINFO_STATUS_RPN = 0x20,
   CLAP_MIDIINFO_STATUS_NRPN = 0x30,
   CLAP_MIDIINFO_STATUS_PER_NOTE_PITCHBEND = 0x60,
   CLAP_MIDIINFO_STATUS_POLY_AFTERTOUCH = 0xA0,
   CLAP_MIDIINFO_STATUS_CONTROLLER = 0xB0,
   CLAP_MIDIINFO_STATUS_CHANNEL_AFTERTOUCH = 0xD0,
   CLAP_MIDIINFO_STATUS_PITCHBEND = 0xE0,

   //CLAP note expressions.
   //A host may map MIDI messages to note expressions, so it needs to know which note expressions are supported.
   CLAP_MIDIINFO_STATUS_NOTE_EXPRESSION_VOLUME = 0x100,
   CLAP_MIDIINFO_STATUS_NOTE_EXPRESSION_PAN = 0x101,
   CLAP_MIDIINFO_STATUS_NOTE_EXPRESSION_TUNING = 0x102,
   CLAP_MIDIINFO_STATUS_NOTE_EXPRESSION_VIBRATO = 0x103,
   CLAP_MIDIINFO_STATUS_NOTE_EXPRESSION_EXPRESSION = 0x104,
   CLAP_MIDIINFO_STATUS_NOTE_EXPRESSION_BRIGHTNESS = 0x105,
   CLAP_MIDIINFO_STATUS_NOTE_EXPRESSION_PRESSURE = 0x106
};

/* This describes a MIDI control/message */
typedef struct clap_midiinfo {
   //CLAP_MIDIINFO_STATUS, describes what kind of control this struct is about.
   status: uint32_t;        

   //controller number, nrpn number etc.
   //zero if not applicable the status value
   number: uint32_t;

   //Display name
   //If name is empty the host can assume the standard MIDI name
   char name[CLAP_NAME_SIZE];
} clap_midiinfo_t;

typedef struct clap_plugin_midiinfo {
   // Returns the number of clap_midiinfo structs for a channel.
   // channel is 0..15
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin,
                             uint32_t             channel);

   // Fills midiinfo. Returns true on success.
   // Important: the most important controls (from a performing musician's point of view) should be listed first,
   // so the host can make sure these appear on a controller keyboard.
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_plugin_t *plugin,
                            uint32_t             channel,
                            uint32_t             index,
                            clap_param_info_t   *midiinfo);

   //if false all channels are the same, so the host doesn't have to scan them all.
   bool(CLAP_ABI *multitimbral)(const clap_plugin_t *plugin);
} clap_plugin_midiinfo_t;

typedef struct clap_host_midiinfo {
   // Rescan the full list of structs.
   // This can happen if an instrument switches to a different patch, for example.
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host);
} clap_host_midiinfo_t;

#ifdef __cplusplus
}
#endif