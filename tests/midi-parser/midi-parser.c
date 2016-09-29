#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include <clap/helpers/midi-parser.h>

void usage(const char *prog)
{
  printf("usage: %s <file.midi>\n", prog);
}

void parse_and_dump(struct clap_midi_parser *parser)
{
  enum clap_midi_channel_event_type status;

  while (1) {
    status = clap_midi_parse(parser);
    switch (status) {
    case CLAP_MIDI_PARSER_EOB:
      puts("eob");
      return;

    case CLAP_MIDI_PARSER_ERROR:
      puts("error");
      return;

    case CLAP_MIDI_PARSER_INIT:
      puts("init");
      break;

    case CLAP_MIDI_PARSER_HEADER:
      printf("header\n");
      printf("  size: %d\n", parser->header.size);
      printf("  format: %d\n", parser->header.format);
      printf("  tracks count: %d\n", parser->header.tracks_count);
      printf("  time division: %d\n", parser->header.time_division);
      break;

    case CLAP_MIDI_PARSER_TRACK:
      puts("track");
      break;

    case CLAP_MIDI_PARSER_TRACK_MIDI:
      puts("track-midi");
      printf("  event type: %d\n", parser->channel.event_type);
      printf("  channel: %d\n", parser->channel.channel);
      printf("  param1: %d\n", parser->channel.param1);
      printf("  param2: %d\n", parser->channel.param2);
      break;

    case CLAP_MIDI_PARSER_TRACK_META:
      printf("track-meta\n");
      printf("  type: %d\n", parser->meta.type);
      printf("  length: %d\n", parser->meta.length);
      break;

    case CLAP_MIDI_PARSER_TRACK_SYSEX:
      puts("track-sysex");
      break;

    default:
      printf("unhandled state: %d\n", status);
      return;
    }
  }
}

int parse_file(const char *path)
{
  struct stat st;

  if (stat(path, &st)) {
    printf("stat(%s): %m\n", path);
    return 1;
  }

  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    printf("open(%s): %m\n", path);
    return 1;
  }

  void *mem = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) {
    printf("mmap(%s): %m\n", path);
    close(fd);
    return 1;
  }

  struct clap_midi_parser parser;
  parser.state = CLAP_MIDI_PARSER_INIT;
  parser.size = st.st_size;
  parser.in   = mem;

  parse_and_dump(&parser);

  munmap(mem, st.st_size);
  close(fd);
  return 0;
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    usage(argv[0]);
    return 1;
  }

  return parse_file(argv[1]);
}
