#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_EVENT_NOTE_ON,         // note attribute
   CLAP_EVENT_NOTE_OFF,        // note attribute
   CLAP_EVENT_NOTE_EXPRESSION, // note_expression attribute
   CLAP_EVENT_CHOKE,           // no attribute
   CLAP_EVENT_PARAM_SET,       // param attribute
   CLAP_EVENT_TRANSPORT,       // transport attribute
   CLAP_EVENT_CHORD,           // chord attribute
   CLAP_EVENT_MIDI,            // midi attribute
   CLAP_EVENT_MIDI_SYSEX,      // midi attribute
};
typedef int32_t  clap_event_type;
typedef uint32_t clap_param_id;

/** Note On/Off event. */
typedef struct clap_event_note {
   int32_t key;      // 0..127
   int32_t channel;  // 0..15
   double  velocity; // 0..1
} clap_event_note;

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
   clap_note_expression expression_id;
   int32_t              key;     // 0..127, or -1 to match all keys
   int32_t              channel; // 0..15, or -1 to match all channels
   double               value;   // see expression for the range
   double               ramp;
} clap_event_note_expression;

typedef union clap_param_value {
   bool    b;
   double  d;
   int64_t i;
} clap_param_value;

typedef struct clap_event_param {
   int32_t          key;
   int32_t          channel;
   clap_param_id    param_id; // parameter index
   clap_param_value value;
   double           ramp; // valid until the end of the block or the next event
} clap_event_param;

typedef struct clap_event_transport {
   bool has_second_timeline;
   bool has_beats_timeline;
   bool has_time_signature;

   double song_pos_beats;   // position in beats
   double song_pos_seconds; // position in seconds

   double tempo;      // in bpm
   double tempo_ramp; // tempo increment for each samples and until the next
                      // time info event

   double  bar_start;  // start pos of the current bar
   int32_t bar_number; // bar at song pos 0 has the number 0

   bool is_playing;
   bool is_recording;

   bool   is_loop_active;
   double loop_start_beats;
   double loop_end_beats;
   double loop_start_seconds;
   double loop_end_seconds;

   int16_t tsig_num;   // time signature numerator
   int16_t tsig_denom; // time signature denominator
} clap_event_transport;

typedef struct clap_event_chord {
   // bitset of active keys:
   // - 11 bits
   // - root note is not part of the bitset
   // - bit N is: root note + N + 1
   // 0000 0000 0100 0100 -> minor chord
   // 0000 0000 0100 1000 -> major chord
   uint16_t note_mask;
   uint8_t  root_note; // 0..11, 0 for C
} clap_event_chord;

typedef struct clap_event_midi {
   uint8_t data[4];
} clap_event_midi;

typedef struct clap_event_midi_sysex {
   const uint8_t *buffer; // midi buffer
   uint32_t       size;
} clap_event_midi_sysex;

typedef struct clap_event {
   clap_event_type type;
   uint32_t        time; // offset from the first sample in the process block

   union {
      clap_event_note            note;
      clap_event_note_expression note_expression;
      clap_event_param           param;
      clap_event_transport       time_info;
      clap_event_chord           chord;
      clap_event_midi            midi;
      clap_event_midi_sysex      midi_sysex;
   };
} clap_event;

typedef struct clap_event_list {
   void *ctx; // reserved pointer for the list

   uint32_t (*size)(const struct clap_event_list *list);

   // Don't free the return event, it belongs to the list
   const clap_event *(*get)(const struct clap_event_list *list, uint32_t index);

   // Makes a copy of the event
   void (*push_back)(const struct clap_event_list *list, const clap_event *event);
} clap_event_list;

#ifdef __cplusplus
}
#endif