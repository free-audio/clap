/*
 * CLAP - CLever Audio Plugin
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyright (c) 2014...2021 Alexandre BIQUE <bique.alexandre@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_VERSION_MAKE(Major, Minor, Revision)                              \
   ((((Major)&0xff) << 16) | (((Minor)&0xff) << 8) | ((Revision)&0xff))
#define CLAP_VERSION CLAP_VERSION_MAKE(0, 3, 0)
#define CLAP_VERSION_MAJ(Version) (((Version) >> 16) & 0xff)
#define CLAP_VERSION_MIN(Version) (((Version) >> 8) & 0xff)
#define CLAP_VERSION_REV(Version) ((Version)&0xff)

#if defined _WIN32 || defined __CYGWIN__
#   ifdef __GNUC__
#      define CLAP_EXPORT __attribute__((dllexport))
#   else
#      define CLAP_EXPORT __declspec(dllexport)
#   endif
#else
#   if __GNUC__ >= 4 || defined(__clang__)
#      define CLAP_EXPORT __attribute__((visibility("default")))
#   else
#      define CLAP_EXPORT
#   endif
#endif

typedef enum clap_string_size {
   CLAP_ID_SIZE = 128,
   CLAP_NAME_SIZE = 64,
} clap_string_size;

// Description of the plugin
#define CLAP_ATTR_DESCRIPTION "clap/description"
// Manufacturer name
#define CLAP_ATTR_MANUFACTURER "clap/manufacturer"
// Url to product
#define CLAP_ATTR_URL "clap/url"
// Url to support page, or mail to support
#define CLAP_ATTR_SUPPORT "clap/support"

////////////////
// PARAMETERS //
////////////////

typedef union clap_param_value {
   bool    b;
   double  d;
   int64_t i;
} clap_param_value;

///////////////
// TRANSPORT //
///////////////

typedef struct clap_loop_info {
   bool   is_loop_active;
   double loop_start;
   double loop_end;
} clap_loop_info;

////////////
// EVENTS //
////////////

typedef enum clap_event_type {
   CLAP_EVENT_NOTE_ON,         // note attribute
   CLAP_EVENT_NOTE_OFF,        // note attribute
   CLAP_EVENT_NOTE_EXPRESSION, // note_expression attribute
   CLAP_EVENT_CHOKE,           // no attribute
   CLAP_EVENT_PARAM_SET,       // param attribute
   CLAP_EVENT_JUMP,            // jump attribute
   CLAP_EVENT_TEMPO,           // tempo attribute
   CLAP_EVENT_PLAY,            // is_playing attribute
   CLAP_EVENT_RECORD,          // is_recording attribute
   CLAP_EVENT_TSIG,            // tsig attribute
   CLAP_EVENT_CHORD,           // chord attribute

   /* MIDI Style */
   CLAP_EVENT_PROGRAM,    // program attribute
   CLAP_EVENT_MIDI,       // midi attribute
   CLAP_EVENT_MIDI_SYSEX, // midi attribute
} clap_event_type;

/** Note On/Off event. */
typedef struct clap_event_note {
   int32_t key;      // 0..127
   int32_t channel;  // 0..15
   double  velocity; // 0..1
} clap_event_note;

typedef enum clap_note_expression {
   // TODO range, 20 * log(K * x)?
   CLAP_NOTE_EXPRESSION_VOLUME,

   // pan, -1 left, 1 right
   CLAP_NOTE_EXPRESSION_PAN,

   // relative tunind in semitone
   CLAP_NOTE_EXPRESSION_TUNING,
   CLAP_NOTE_EXPRESSION_VIBRATO,
   CLAP_NOTE_EXPRESSION_BRIGHTNESS,
   CLAP_NOTE_EXPRESSION_BREATH,
   CLAP_NOTE_EXPRESSION_PRUSSURE,
   CLAP_NOTE_EXPRESSION_TIMBRE,

   // TODO...
} clap_note_expression;

typedef struct clap_event_note_expression {
   clap_note_expression expression_id;
   int32_t              key;         // 0..127, or -1 to match all keys
   int32_t              channel;     // 0..15, or -1 to match all channels
   int32_t              control;     // 0..127
   double               normalized_value; // see expression for the range
   double               normalized_ramp;
} clap_event_note_expression;

typedef struct clap_event_param {
   int32_t          key;
   int32_t          channel;
   uint32_t         index; // parameter index
   clap_param_value normalized_value;
   double normalized_ramp; // valid until the end of the block or the next event
} clap_event_param;

typedef struct clap_event_jump {
   double song_pos_beats;   // position in beats
   double song_pos_seconds; // position in seconds

   double bar_start; // start pos of the current bar
   int32_t bar_number; // bar at song pos 0 has the number 0
} clap_event_jump;

typedef struct clap_event_tsig {
   int16_t num;   // time signature numerator
   int16_t denom; // time signature denominator
} clap_event_tsig;

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

/**
 * Asks the plugin to load a program.
 * This is analogue to the midi program change.
 *
 * The main advantage of setting a program instead of loading
 * a preset, is that the program should already be in the plugin's
 * memory, and can be set instantly (no loading time).
 */
typedef struct clap_event_program {
   int32_t channel;  // 0..15, -1 unspecified
   int32_t bank_msb; // 0..0x7FFFFFFF, -1 unspecified
   int32_t bank_lsb; // 0..0x7FFFFFFF, -1 unspecified
   int32_t program;  // 0..0x7FFFFFFF
} clap_event_program;

typedef struct clap_event {
   clap_event_type type;
   uint32_t        time; // offset from the first sample in the process block

   union {
      clap_event_note            note;
      clap_event_note_expression note_expression;
      clap_event_param           param;
      clap_event_jump            jump;
      clap_event_chord           chord;
      double                     tempo;
      clap_event_tsig            tsig;
      bool                       is_playing;
      bool                       is_recording;
      clap_event_program         program;
      clap_event_midi            midi;
      clap_event_midi_sysex      midi_sysex;
   };
} clap_event;

typedef struct clap_event_list {
   void *ctx;

   uint32_t (*size)(const clap_event_list *list);

   // Don't free the return event, it belongs to the list
   const clap_event *(*get)(const clap_event_list *list, uint32_t index);

   // Makes a copy of the event
   void (*push_back)(const clap_event_list *list, const clap_event *event);
} clap_event_list;

/////////////
// PROCESS //
/////////////

typedef enum clap_process_status {
   // Processing failed. The output buffer must be discarded.
   CLAP_PROCESS_ERROR = 0,

   // Processing succeed.
   CLAP_PROCESS_CONTINUE = 1,

   // Processing succeed, but no more processing is required, until next event.
   CLAP_PROCESS_SLEEP = 2,
} clap_process_status;

typedef struct clap_audio_buffer {
   // Either data32 or data64 will be set, but not both.
   // If none are set, assume that the input has the value 0 for each samples.
   // data[i] for channel i buffer
   float ** data32;
   double **data64;
   int32_t  channel_count;
   uint32_t latency;       // latency from/to the audio interface
   uint64_t constant_mask; // bitmask for each channel, 1 if the value is
                           // constant for the whole buffer
   const char *plugin_port_id; // optional, used for validation
} clap_audio_buffer;

typedef struct clap_process {
   int64_t steady_time;   // a steady sample time counter, requiered
   int32_t frames_count;  // number of frame to process
   bool    has_transport; // if false then this is a free running host, no
                          // transport events will be provided

   clap_loop_info loop_info; // only valid if has_transport is true

   // Audio buffers, they must have the same count as specified
   // by clap_plugin_audio_ports->get_count().
   // The index maps to clap_plugin_audio_ports->get_info().
   //
   // If a plugin does not implement clap_plugin_audio_ports,
   // then it gets a default stereo input and output.
   const clap_audio_buffer *audio_inputs;
   const clap_audio_buffer *audio_outputs;
   int32_t                  audio_inputs_count;
   int32_t                  audio_outputs_count;

   /* events */
   const clap_event_list *in_events;
   const clap_event_list *out_events;
} clap_process;

//////////
// HOST //
//////////

typedef struct clap_host {
   int32_t clap_version; // initialized to CLAP_VERSION

   void *host_data; // reserved pointer for the host

   /* Name and version are mandatory. */
   char name[CLAP_NAME_SIZE];    // plugin name, eg: "BitwigStudio"
   char version[CLAP_NAME_SIZE]; // the plugin version, eg: "1.3.14"

   /* Returns the size of the original string. If this is larger than size, then
    * the function did not have enough space to copy all the data.
    * [thread-safe] */
   int32_t (*get_attribute)(clap_host * host,
                            const char *attr,
                            char *      buffer,
                            int32_t     size);

   /* Query an extension.
    * [thread-safe] */
   const void *(*extension)(clap_host *host, const char *extension_id);
} clap_host;

////////////
// PLUGIN //
////////////

/* bitfield
 * This gives an hint to the host what the plugin might do. */
typedef enum clap_plugin_type {
   /* Instruments can play notes, and generate audio */
   CLAP_PLUGIN_INSTRUMENT = (1 << 0),

   /* Audio effects, process audio input and produces audio.
    * Exemple: delay, reverb, compressor. */
   CLAP_PLUGIN_AUDIO_EFFECT = (1 << 1),

   /* Event effects, takes events as input and produces events.
    * Exemple: arpegiator */
   CLAP_PLUGIN_EVENT_EFFECT = (1 << 2), // can be seen as midi effect

   /* Analyze audio and/or events, and produces analysis results,
    * but doesn't change audio. */
   CLAP_PLUGIN_ANALYZER = (1 << 3),
} clap_plugin_type;

typedef struct clap_plugin {
   int32_t clap_version; // initialized to CLAP_VERSION

   void *plugin_data; // reserved pointer for the plugin

   /* The 3 following strings are here because:
    * - they are mandatory
    * - it is convenient when you debug, to be able to see
    *   the plugin name, id and version by displaying
    *   the structure.
    */
   char name[CLAP_NAME_SIZE];    // plugin name, eg: "Diva"
   char id[CLAP_ID_SIZE];        // plugin id, eg: "com.u-he.diva"
   char version[CLAP_NAME_SIZE]; // the plugin version, eg: "1.3.2"

   uint64_t plugin_type; // bitfield of clap_plugin_type

   /* Free the plugin and its resources.
    * It is not required to deactivate the plugin prior to this call. */
   void (*destroy)(clap_plugin *plugin);

   /* Copy at most size of the attribute's value into buffer.
    * This function must place a '\0' byte at the end of the string.
    * Returns the size of the original string or 0 if there is no
    * value for this attributes.
    * [thread-safe] */
   int32_t (*get_attribute)(clap_plugin *plugin,
                            const char * attr,
                            char *       buffer,
                            int32_t      size);

   /* activation/deactivation
    * [main-thread] */
   bool (*activate)(clap_plugin *plugin, int sample_rate);
   void (*deactivate)(clap_plugin *plugin);

   /* process audio, events, ...
    * [audio-thread] */
   clap_process_status (*process)(clap_plugin *       plugin,
                                  const clap_process *process);

   /* query an extension
    * [thread-safe] */
   const void *(*extension)(clap_plugin *plugin, const char *id);
} clap_plugin;

/* This interface is the entry point of the dynamic library.
 *
 * Every methods must be thread-safe. */
struct clap_plugin_entry {
   void (*init)(const char *plugin_path);
   void (*deinit)(void);

   /* Get the number of plugins available.
    * [thread-safe] */
   int32_t (*get_plugin_count)(void);

   /* Create a clap_plugin by its index.
    * Returns null in case of error.
    * [thread-safe] */
   clap_plugin *(*create_plugin_by_index)(clap_host *host, int32_t index);

   /* Create a clap_plugin by its plugin_id.
    * Returns null in case of error.
    * [thread-safe] */
   clap_plugin *(*create_plugin_by_id)(clap_host *host, const char *plugin_id);
};

/* Entry point */
CLAP_EXPORT extern const struct clap_plugin_entry clap_plugin_entry;

#ifdef __cplusplus
}
#endif
