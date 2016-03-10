/*
 * Simple MIDI parser implementation.
 * I used the following reference:
 * http://www.sonicspot.com/guide/midifiles.html
 */

#ifndef CLAP_HELPERS_MIDI_PARSER_H
# define CLAP_HELPERS_MIDI_PARSER_H

# include <stdint.h>
# include <string.h>

# include "../clap.h"

enum clap_midi_parser_status
{
  CLAP_MIDI_PARSER_EOB     = -2,
  CLAP_MIDI_PARSER_ERROR   = -1,
  CLAP_MIDI_PARSER_INIT    = 0,
  CLAP_MIDI_PARSER_HEADER  = 1,
  CLAP_MIDI_PARSER_TRACK   = 2,
  CLAP_MIDI_PARSER_CHANNEL = 3,
  CLAP_MIDI_PARSER_META    = 4,
  CLAP_MIDI_PARSER_SYSEX   = 5,
};

struct clap_midi_header
{
  int32_t size;
  uint16_t format;
  int16_t tracks_count;
  int16_t time_division;
};

struct clap_midi_track
{
  int32_t size;
};

enum clap_midi_channel_event_type
{
  CLAP_MIDI_CHANNEL_NOTE_OFF   = 0x8,
  CLAP_MIDI_CHANNEL_NOTE_ON    = 0x9,
  CLAP_MIDI_CHANNEL_NOTE_AT    = 0xA, // after touch
  CLAP_MIDI_CHANNEL_CC         = 0xB, // control change
  CLAP_MIDI_CHANNEL_PGM_CHANGE = 0xC,
  CLAP_MIDI_CHANNEL_CHANNEL_AT = 0xD, // after touch
  CLAP_MIDI_CHANNEL_PITCH_BEND = 0xF,
};

struct clap_midi_channel_event
{
  int64_t  delta_time;
  unsigned event_type : 4;
  unsigned channel : 4;
  uint8_t  param1;
  uint8_t  param2;
};

struct clap_midi_meta_event
{
  uint8_t        type;
  int32_t        length;
  const uint8_t *bytes;  // reference to the input buffer
};

struct clap_midi_sysex_event
{
  uint8_t        sysex;
  uint8_t        type;
  int32_t        length;
  const uint8_t *bytes;  // reference to the input buffer
};

struct clap_midi_parser
{
  enum clap_midi_parser_status state;

  /* input buffer */
  const uint8_t *in;
  int32_t        size;

  /* result */
  struct clap_midi_header        header;
  struct clap_midi_track         track;
  struct clap_midi_channel_event channel;
  struct clap_midi_meta_event    meta;
  struct clap_midi_sysex_event   sysex;
};

static inline enum clap_midi_parser_status
clap_midi_parse(struct clap_midi_parser *parser);

/* Converts a midi buffer in the state track, into a clap_event.
 * If the midi data can't be converted into clap's events, it is then
 * converted as a clap_midi_event. */
static inline void
clap_midi_convert(const uint8_t     *in,
                  int32_t            size,
                  struct clap_event *event);

# include "parser.c"

#endif /* !CLAP_MIDI_PARSER_H */
