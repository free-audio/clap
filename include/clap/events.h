#pragma once

#include "private/std.h"

#include "fixedpoint.h"
#include "id.h"
#include "private/align.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, CLAP_ALIGN)

// event header
// must be the first attribute of the event
typedef struct clap_event_header {
   alignas(4) uint32_t size;     // event size including this header, eg: sizeof (clap_event_note)
   alignas(4) uint32_t time;     // time at which the event happens
   alignas(2) uint16_t space_id; // event space, see clap_host_event_registry
   alignas(2) uint16_t type;     // event type
   alignas(4) uint32_t flags;    // see clap_event_flags
} clap_event_header_t;

// The clap core event space
static const CLAP_CONSTEXPR uint16_t CLAP_CORE_EVENT_SPACE_ID = 0;

enum clap_event_flags {
   // indicate a live momentary event
   CLAP_EVENT_IS_LIVE = 1 << 0,

   // live user adjustment begun
   CLAP_EVENT_BEGIN_ADJUST = 1 << 1,

   // live user adjustment ended
   CLAP_EVENT_END_ADJUST = 1 << 2,

   // should record this event be recorded?
   CLAP_EVENT_SHOULD_RECORD = 1 << 3,
};

// Some of the following events overlap, a note on can be expressed with:
// - CLAP_EVENT_NOTE_ON
// - CLAP_EVENT_MIDI
// - CLAP_EVENT_MIDI2
//
// The preferred way of sending a note event is to use CLAP_EVENT_NOTE_*.
//
// The same event must not be sent twice: it is forbidden to send a the same note on
// encoded with both CLAP_EVENT_NOTE_ON and CLAP_EVENT_MIDI.
//
// The plugins are encouraged to be able to handle note events encoded as raw midi or midi2,
// or implement clap_plugin_event_filter and reject raw midi and midi2 events.
enum {
   // NOTE_ON and NOTE_OFF represents a key pressed and key released event.
   //
   // NOTE_CHOKE is meant to choke the voice(s), like in a drum machine when a closed hihat
   // chokes an open hihat.
   //
   // NOTE_END is sent by the plugin to the host, when a voice terminates.
   // When using polyphonic modulations, the host has to start voices for its modulators.
   // This message helps the host to track the plugin's voice management.
   //
   // Those four events use the note attribute.
   CLAP_EVENT_NOTE_ON,
   CLAP_EVENT_NOTE_OFF,
   CLAP_EVENT_NOTE_CHOKE,
   CLAP_EVENT_NOTE_END,

   // Represents a note expression.
   // Uses the note_expression attribute.
   CLAP_EVENT_NOTE_EXPRESSION,

   // PARAM_VALUE sets the parameter's value; uses param_value attribute
   // PARAM_MOD sets the parameter's modulation amount; uses param_mod attribute
   //
   // The value heard is: param_value + param_mod.
   //
   // In case of a concurrent global value/modulation versus a polyphonic one,
   // the voice should only use the polyphonic one and the polyphonic modulation
   // amount will already include the monophonic signal.
   CLAP_EVENT_PARAM_VALUE,
   CLAP_EVENT_PARAM_MOD,

   CLAP_EVENT_TRANSPORT,  // update the transport info; transport attribute
   CLAP_EVENT_MIDI,       // raw midi event; midi attribute
   CLAP_EVENT_MIDI_SYSEX, // raw midi sysex event; midi_sysex attribute
   CLAP_EVENT_MIDI2,      // raw midi 2 event; midi2 attribute
};
typedef int32_t clap_event_type;

/**
 * Note on, off, end and choke events.
 * In the case of note choke or end events:
 * - the velocity is ignored.
 * - key and channel are used to match active notes, a value of -1 matches all.
 */
typedef struct clap_event_note {
   alignas(4) clap_event_header_t header;

   alignas(2) int16_t port_index;
   alignas(2) int16_t key;     // 0..127
   alignas(2) int16_t channel; // 0..15
   alignas(8) double velocity; // 0..1
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
   alignas(4) clap_event_header_t header;

   alignas(4) clap_note_expression expression_id;

   // target a specific port, key and channel, -1 for global
   alignas(2) int16_t port_index;
   alignas(2) int16_t key;
   alignas(2) int16_t channel;

   alignas(8) double value; // see expression for the range
} clap_event_note_expression_t;

typedef struct clap_event_param_value {
   alignas(4) clap_event_header_t header;

   // target parameter
   void *cookie;                // @ref clap_param_info.cookie
   alignas(4) clap_id param_id; // @ref clap_param_info.id

   // target a specific port, key and channel, -1 for global
   alignas(2) int16_t port_index;
   alignas(2) int16_t key;
   alignas(2) int16_t channel;

   alignas(8) double value;
} clap_event_param_value_t;

typedef struct clap_event_param_mod {
   alignas(4) clap_event_header_t header;

   // target parameter
   alignas(4) clap_id param_id; // @ref clap_param_info.id
   void *cookie;                // @ref clap_param_info.cookie

   // target a specific port, key and channel, -1 for global
   alignas(2) int16_t port_index;
   alignas(2) int16_t key;
   alignas(2) int16_t channel;

   alignas(8) double amount; // modulation amount
} clap_event_param_mod_t;

enum clap_transport_flags {
   CLAP_TRANSPORT_HAS_TEMPO = 1 << 0,
   CLAP_TRANSPORT_HAS_BEATS_TIMELINE = 1 << 1,
   CLAP_TRANSPORT_HAS_SECONDS_TIMELINE = 1 << 2,
   CLAP_TRANSPORT_HAS_TIME_SIGNATURE = 1 << 3,
   CLAP_TRANSPORT_IS_PLAYING = 1 << 4,
   CLAP_TRANSPORT_IS_RECORDING = 1 << 5,
   CLAP_TRANSPORT_IS_LOOP_ACTIVE = 1 << 6,
   CLAP_TRANSPORT_IS_WITHIN_PRE_ROLL = 1 << 7,
};

typedef struct clap_event_transport {
   alignas(4) clap_event_header_t header;

   alignas(4) uint32_t flags; // see clap_transport_flags

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

typedef struct clap_event_midi {
   alignas(4) clap_event_header_t header;

   alignas(2) uint16_t port_index;
   alignas(1) uint8_t data[3];
} clap_event_midi_t;

typedef struct clap_event_midi_sysex {
   alignas(4) clap_event_header_t header;

   alignas(2) uint16_t port_index;
   const uint8_t *buffer; // midi buffer
   alignas(4) uint32_t size;
} clap_event_midi_sysex_t;

typedef struct clap_event_midi2 {
   alignas(4) clap_event_header_t header;

   alignas(2) uint16_t port_index;
   alignas(4) uint32_t data[4];
} clap_event_midi2_t;

// Input event list, events must be sorted by time.
typedef struct clap_input_events {
   void *ctx; // reserved pointer for the list

   uint32_t (*size)(const struct clap_input_events *list);

   // Don't free the return event, it belongs to the list
   const clap_event_header_t *(*get)(const struct clap_input_events *list, uint32_t index);
} clap_input_events_t;

// Output event list, events must be sorted by time.
typedef struct clap_output_events {
   void *ctx; // reserved pointer for the list

   // Pushes a copy of the event
   void (*push_back)(const struct clap_output_events *list, const clap_event_header_t *event);
} clap_output_events_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif