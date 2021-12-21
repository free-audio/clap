#pragma once

#include "private/std.h"

#include "fixedpoint.h"
#include "id.h"
#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

enum {
   CLAP_EVENT_NOTE_ON,    // press a key; note attribute
   CLAP_EVENT_NOTE_OFF,   // release a key; note attribute
   CLAP_EVENT_NOTE_END,   // playback of a note is terminated (sent by the plugin); note attribute
   CLAP_EVENT_NOTE_CHOKE, // chokes a set of notes; note attribute
   CLAP_EVENT_NOTE_EXPRESSION, // plays standard note expression; note_expression attribute
   CLAP_EVENT_NOTE_MASK,       // current chord/scale; note_mask attribute
   CLAP_EVENT_PARAM_VALUE,     // sets a parameter value; param_value attribute
   CLAP_EVENT_PARAM_MOD,       // sets a parameter modulation; param_mod attribute
   CLAP_EVENT_TRANSPORT,       // update the transport info; transport attribute
   CLAP_EVENT_MIDI,            // raw midi event; midi attribute
   CLAP_EVENT_MIDI_SYSEX,      // raw midi sysex event; midi_sysex attribute
};
typedef int32_t clap_event_type;

/**
 * Note on, off, end and choke events.
 * In the case of note choke or end events:
 * - the velocity is ignored.
 * - key and channel are used to match active notes, a value of -1 matches all.
 */
typedef struct clap_event_note {
   int32_t port_index;
   int32_t key;      // 0..127
   int32_t channel;  // 0..15
   double  velocity; // 0..1
} clap_event_note_t;

enum {
   // x >= 0, use 20 * log(4 * x)
   CLAP_NOTE_EXPRESSION_VOLUME,

   // pan, 0 left, 0.5 center, 1 right
   CLAP_NOTE_EXPRESSION_PAN,

   // relative tuning in semitone, from -120 to +120
   CLAP_NOTE_EXPRESSION_TUNING,

   // 0..1
   CLAP_NOTE_EXPRESSION_VIBRATO,
   CLAP_NOTE_EXPRESSION_BRIGHTNESS,
   CLAP_NOTE_EXPRESSION_BREATH,
   CLAP_NOTE_EXPRESSION_PRESSURE,
   CLAP_NOTE_EXPRESSION_TIMBRE,

   // TODO...
};
typedef int32_t clap_note_expression;

typedef struct clap_event_note_expression {
   alignas(4) clap_note_expression expression_id;

   // target a specific port, key and channel, -1 for global
   alignas(4) int32_t port_index;
   alignas(4) int32_t key;
   alignas(4) int32_t channel;

   alignas(8) double value; // see expression for the range
} clap_event_note_expression_t;

enum {
   // live user adjustment begun
   CLAP_EVENT_PARAM_BEGIN_ADJUST = 1 << 0,

   // live user adjustment ended
   CLAP_EVENT_PARAM_END_ADJUST = 1 << 1,

   // should record this parameter change and create an automation point?
   CLAP_EVENT_PARAM_SHOULD_RECORD = 1 << 2,
};
typedef int32_t clap_event_param_flags;

typedef struct clap_event_param_value {
   // target parameter
   void *cookie;                // @ref clap_param_info.cookie
   alignas(4) clap_id param_id; // @ref clap_param_info.id

   // target a specific port, key and channel, -1 for global
   alignas(4) int32_t port_index;
   alignas(4) int32_t key;
   alignas(4) int32_t channel;

   alignas(4) clap_event_param_flags flags;

   alignas(8) double value;
} clap_event_param_value_t;

typedef struct clap_event_param_mod {
   // target parameter
   void *cookie;                // @ref clap_param_info.cookie
   alignas(4) clap_id param_id; // @ref clap_param_info.id

   // target a specific port, key and channel, -1 for global
   alignas(4) int32_t port_index;
   alignas(4) int32_t key;
   alignas(4) int32_t channel;

   alignas(8) double amount; // modulation amount
} clap_event_param_mod_t;

enum {
   CLAP_TRANSPORT_HAS_TEMPO = 1 << 0,
   CLAP_TRANSPORT_HAS_BEATS_TIMELINE = 1 << 1,
   CLAP_TRANSPORT_HAS_SECONDS_TIMELINE = 1 << 2,
   CLAP_TRANSPORT_HAS_TIME_SIGNATURE = 1 << 3,
   CLAP_TRANSPORT_IS_PLAYING = 1 << 4,
   CLAP_TRANSPORT_IS_RECORDING = 1 << 5,
   CLAP_TRANSPORT_IS_LOOP_ACTIVE = 1 << 6,
   CLAP_TRANSPORT_IS_WITHIN_PRE_ROLL = 1 << 7,
};
typedef uint32_t clap_transport_flags;

typedef struct clap_event_transport {
   alignas(4) clap_transport_flags flags;

   alignas(8) clap_beattime song_pos_beats;  // position in beats
   alignas(8) clap_sectime song_pos_seconds; // position in seconds

   alignas(8) double tempo;     // in bpm
   alignas(8) double tempo_inc; // tempo increment for each samples and until the next
                                // time info event

   alignas(8) clap_beattime bar_start; // start pos of the current bar
   alignas(4) int32_t bar_number;      // bar at song pos 0 has the number 0

   alignas(8) clap_beattime loop_start_beats;
   alignas(8) clap_beattime loop_end_beats;
   alignas(8) clap_sectime loop_start_seconds;
   alignas(8) clap_sectime loop_end_seconds;

   alignas(2) int16_t tsig_num;   // time signature numerator
   alignas(2) int16_t tsig_denom; // time signature denominator
} clap_event_transport_t;

typedef struct clap_event_note_mask {
   uint32_t port_index;

   // bitset of active keys:
   // - 11 bits
   // - root note is not part of the bitset
   // - bit N is: root note + N + 1
   // 000 0100 0100 -> minor chord
   // 000 0100 1000 -> major chord
   // 010 1011 0101 -> locrian scale
   alignas(2) uint16_t note_mask;
   alignas(1) uint8_t root_note; // 0..11, 0 for C
} clap_event_note_mask_t;

typedef struct clap_event_midi {
   alignas(4) uint32_t port_index;
   alignas(1) uint8_t data[3];
} clap_event_midi_t;

typedef struct clap_event_midi_sysex {
   alignas(4) uint32_t port_index;
   const uint8_t *buffer; // midi buffer
   alignas(4) uint32_t size;
} clap_event_midi_sysex_t;

typedef struct clap_event {
   alignas(4) clap_event_type type;
   alignas(4) uint32_t        time; // offset from the first sample in the process block

   union {
      clap_event_note_t            note;
      clap_event_note_expression_t note_expression;
      clap_event_param_value_t     param_value;
      clap_event_param_mod_t       param_mod;
      clap_event_transport_t       time_info;
      clap_event_note_mask_t       note_mask;
      clap_event_midi_t            midi;
      clap_event_midi_sysex_t      midi_sysex;
   };
} clap_event_t;

typedef struct clap_event_list {
   void *ctx; // reserved pointer for the list

   uint32_t (*size)(const struct clap_event_list *list);

   // Don't free the return event, it belongs to the list
   const clap_event_t *(*get)(const struct clap_event_list *list, uint32_t index);

   // Makes a copy of the event
   void (*push_back)(const struct clap_event_list *list, const clap_event_t *event);
} clap_event_list_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif