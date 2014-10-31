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

static inline enum clap_midi_parser_status
clap_midi_parse_header(struct clap_midi_parser *parser)
{
  if (paser->size < 14)
    return CLAP_MIDI_PARSER_EOB;

  if (memcmp(parser->in, "MThd"))
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
  if (paser->size < 8)
    return CLAP_MIDI_PARSER_EOB;

  parser->track.size          = clap_midi_parse_be32(parser->in + 4);

  parser->in   += 8;
  parser->size -= 8;
  return CLAP_MIDI_PARSER_HEADER;
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
    return clap_midi_parse_event(parser);

  default:
    return CLAP_MIDI_PARSER_ERROR;
  }
}
