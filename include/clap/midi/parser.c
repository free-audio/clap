#include <assert.h>
#include <math.h>

static inline float
clap_midi_pitch(uint8_t key)
{
  return 440.0f * powf(2, (key - 57.f) / 12.f);
}

// for i in $(seq 0 127);
// do
//   calc "440.0 * 2.0 ^ ((${i}.0 - 57.0) / 12.0)"
// done | sed -r 's/\t(.*)$/  \1,/g'
static const float clap_midi_pitches[128] = {
    16.3515978312874146696,
    17.323914436054506016,
    18.354047994837972516,
    19.4454364826300569232,
    20.6017223070543706096,
    21.8267644645627427796,
    23.1246514194771499336,
    24.499714748859330882,
    25.9565435987465711576,
    27.5,
    29.1352350948806197776,
    30.8677063285077569896,
    32.7031956625748293348,
    34.647828872109012032,
    36.708095989675945032,
    38.890872965260113842,
    41.2034446141087412192,
    43.6535289291254855548,
    46.2493028389542998672,
    48.9994294977186617596,
    51.9130871974931423152,
    55,
    58.2704701897612395508,
    61.7354126570155139792,
    65.4063913251496586696,
    69.295657744218024064,
    73.416191979351890064,
    77.781745930520227684,
    82.406889228217482434,
    87.3070578582509711096,
    92.4986056779085997344,
    97.9988589954373235236,
    103.8261743949862846304,
    110,
    116.5409403795224791016,
    123.4708253140310279584,
    130.8127826502993173392,
    138.5913154884360481236,
    146.8323839587037801324,
    155.563491861040455368,
    164.813778456434964868,
    174.6141157165019422236,
    184.9972113558171994688,
    195.9977179908746470428,
    207.6523487899725692608,
    220,
    233.0818807590449582032,
    246.9416506280620559168,
    261.6255653005986346784,
    277.1826309768720962472,
    293.6647679174075602648,
    311.126983722080910736,
    329.627556912869929736,
    349.2282314330038844472,
    369.9944227116343989332,
    391.9954359817492940856,
    415.3046975799451385216,
    440,
    466.1637615180899164064,
    493.8833012561241118292,
    523.2511306011972693568,
    554.3652619537441924988,
    587.3295358348151205252,
    622.253967444161821472,
    659.255113825739859472,
    698.45646286600776889,
    739.9888454232687978664,
    783.9908719634985881712,
    830.6093951598902770432,
    880,
    932.3275230361798328128,
    987.7666025122482236628,
    1046.5022612023945387092,
    1108.7305239074883849932,
    1174.6590716696302410504,
    1244.507934888323642944,
    1318.510227651479718944,
    1396.91292573201553778,
    1479.9776908465375957328,
    1567.9817439269971763424,
    1661.2187903197805540908,
    1760,
    1864.65504607235966563,
    1975.5332050244964473212,
    2093.0045224047890774228,
    2217.4610478149767699908,
    2349.3181433392604821008,
    2489.0158697766472858924,
    2637.020455302959437888,
    2793.8258514640310755644,
    2959.95538169307519147,
    3135.9634878539943526848,
    3322.4375806395611081816,
    3520,
    3729.3100921447193312556,
    3951.0664100489928946468,
    4186.0090448095781548456,
    4434.9220956299535399816,
    4698.636286678520964206,
    4978.0317395532945717804,
    5274.0409106059188757716,
    5587.6517029280621511244,
    5919.91076338615038294,
    6271.9269757079887053696,
    6644.8751612791222163588,
    7040,
    7458.6201842894386625156,
    7902.1328200979857892936,
    8372.0180896191563096912,
    8869.8441912599070799632,
    9397.2725733570419284076,
    9956.0634791065891435652,
    10548.0818212118377515476,
    11175.3034058561243022532,
    11839.82152677230076588,
    12543.8539514159774107436,
    13289.7503225582444327176,
    14080,
    14917.2403685788773250312,
    15804.2656401959715785828,
    16744.0361792383126193824,
    17739.688382519814159922,
    18794.5451467140838568196,
    19912.126958213178287126,
    21096.1636424236755030952,
    22350.606811712248604502,
    23679.6430535446015317556,
    25087.7079028319548214828,
};

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

  parser->channel.event_type = parser->in[0] >> 4;
  parser->channel.channel    = parser->in[0] & 0xf;
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
  if ((parser->in[0] >> 4) <= 0xe)
    return clap_midi_parse_channel_event(parser);
  if (parser->in[0] == 0xff)
    return clap_midi_parse_meta_event(parser);
  return CLAP_MIDI_PARSER_ERROR;
}

static inline enum clap_midi_parser_status
clap_midi_parse(struct clap_midi_parser *parser)
{
  if (!parser->in || parser->size < 1)
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
      event->note.pitch    = clap_midi_pitches[event->note.key];
      return;

    case CLAP_MIDI_CHANNEL_NOTE_ON:
      event->type          = CLAP_EVENT_NOTE_ON;
      event->note.key      = parser.channel.param1;
      event->note.velocity = ((float)parser.channel.param2) / 127.0f;
      event->note.events   = NULL;
      event->note.pitch    = clap_midi_pitches[event->note.key];
      return;

    case CLAP_MIDI_CHANNEL_CC:
      event->type              = CLAP_EVENT_CONTROL;
      event->control.is_global = false;
      event->control.index     = parser.channel.param1;
      event->control.value     = ((float)parser.channel.param2) / 127.0f;
      return;
    }

    /* fall to default */

  default:
    event->type = CLAP_EVENT_MIDI;
    event->midi.buffer = in;
    event->midi.size   = size;
    return;
  }
}
