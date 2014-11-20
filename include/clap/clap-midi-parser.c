#include <assert.h>

static inline uint16_t
clap_midi_parse_be16(const uint8_t *in)
{
  return (in[0] << 8) | in[1];
}

static inline uint32_t
clap_midi_parse_be32(const uint8_t *in)
{
  return (in[0] << 24) | (in[1] << 16) | (in[2] << 8) | in[3];
}

static inline uint32_t
clap_midi_parse_variable_length(struct clap_midi_parser *parser, uint32_t *offset)
{
  uint32_t value = 0;
  uint32_t i     = *offset;

  for (; i < parser->size; ++i) {
    value = (value << 7) | (parser->in[i] & 0x7f);
    if (!(parser->in[i] & 0x8f))
      break;
  }
  *offset = i + 1;
  return value;
}

static inline enum clap_midi_parser_status
clap_midi_parse_header(struct clap_midi_parser *parser)
{
  if (parser->size < 14)
    return CLAP_MIDI_PARSER_EOB;

  if (memcmp(parser->in, "MThd", 4))
    return CLAP_MIDI_PARSER_ERROR;

  parser->header.size          = clap_midi_parse_be32(parser->in + 4);
  parser->header.format        = clap_midi_parse_be16(parser->in + 8);
  parser->header.tracks_count  = clap_midi_parse_be16(parser->in + 10);
  parser->header.time_division = clap_midi_parse_be16(parser->in + 12);

  parser->in   += 14;
  parser->size -= 14;
  parser->state = CLAP_MIDI_PARSER_HEADER;
  return CLAP_MIDI_PARSER_HEADER;
}

static inline enum clap_midi_parser_status
clap_midi_parse_track(struct clap_midi_parser *parser)
{
  if (parser->size < 8)
    return CLAP_MIDI_PARSER_EOB;

  parser->track.size  = clap_midi_parse_be32(parser->in + 4);
  parser->state       = CLAP_MIDI_PARSER_TRACK;
  parser->in         += 8;
  parser->size       -= 8;
  return CLAP_MIDI_PARSER_TRACK;
}

static inline enum clap_midi_parser_status
clap_midi_parse_channel_event(struct clap_midi_parser *parser)
{
  if (parser->size < 3)
    return CLAP_MIDI_PARSER_EOB;

  parser->channel.event_type = parser->in[0] & 0xf;
  parser->channel.channel    = parser->in[0] >> 4;
  parser->channel.param1     = parser->in[1];
  parser->channel.param2     = parser->in[2];

  parser->in         += 3;
  parser->size       -= 3;
  parser->track.size -= 3;

  return CLAP_MIDI_PARSER_CHANNEL;
}

static inline enum clap_midi_parser_status
clap_midi_parse_meta_event(struct clap_midi_parser *parser)
{
  assert(parser->in[0] == 0xff);

  if (parser->size < 2)
    return CLAP_MIDI_PARSER_EOB;

  parser->meta.type = parser->in[1];
  uint32_t offset   = 2;
  parser->meta.length = clap_midi_parse_variable_length(parser, &offset);

  // check buffer size
  if (parser->size < offset + parser->meta.length)
    return CLAP_MIDI_PARSER_EOB;

  offset += parser->meta.length;
  parser->in += offset;
  parser->size -= offset;
  parser->track.size -= offset;
  return CLAP_MIDI_PARSER_META;
}

static inline enum clap_midi_parser_status
clap_midi_parse_event(struct clap_midi_parser *parser)
{
  if ((parser->in[0] & 0xf) <= 0xe)
    return clap_midi_parse_channel_event(parser);
  if (parser->in[0] == 0xff)
    return clap_midi_parse_meta_event(parser);
  return CLAP_MIDI_PARSER_ERROR;
}

static inline enum clap_midi_parser_status
clap_midi_parse(struct clap_midi_parser *parser)
{
  if (!parser->in || parser->size < 4)
    return CLAP_MIDI_PARSER_EOB;

  switch (parser->state) {
  case CLAP_MIDI_PARSER_INIT:
    return clap_midi_parse_header(parser);

  case CLAP_MIDI_PARSER_HEADER:
    return clap_midi_parse_track(parser);

  case CLAP_MIDI_PARSER_TRACK:
    if (parser->track.size == 0) {
      // we reached the end of the track
      parser->state = CLAP_MIDI_PARSER_HEADER;
      return clap_midi_parse(parser);
    }
    return clap_midi_parse_event(parser);

  default:
    return CLAP_MIDI_PARSER_ERROR;
  }
}

static inline void
clap_midi_convert(const uint8_t     *in,
                  uint32_t           size,
                  struct clap_event *event)
{
  struct clap_midi_parser parser;
  parser.state      = CLAP_MIDI_PARSER_TRACK;
  parser.in         = in;
  parser.size       = size;
  parser.track.size = size;

  enum clap_midi_parser_status status = clap_midi_parse(&parser);
  switch (status) {
  case CLAP_MIDI_PARSER_CHANNEL:
    switch (parser.channel.event_type) {
    case CLAP_MIDI_CHANNEL_NOTE_OFF:
      event->type          = CLAP_EVENT_NOTE_OFF;
      event->note.key      = parser.channel.param1;
      event->note.velocity = ((float)parser.channel.param2) / 127.0f;
      event->note.events   = NULL;
      break;

    case CLAP_MIDI_CHANNEL_NOTE_ON:
      event->type          = CLAP_EVENT_NOTE_ON;
      event->note.key      = parser.channel.param1;
      event->note.velocity = ((float)parser.channel.param2) / 127.0f;
      event->note.events   = NULL;
      break;
    }

  default:
    event->type = CLAP_EVENT_MIDI;
    event->midi.buffer = in;
    event->midi.size   = size;
    break;
  }
}
