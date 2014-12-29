#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

#include <clap/clap.h>
#include <clap/midi/parser.h>
#include <clap/ext/gui.h>

#include <jack/jack.h>
#include <jack/midiport.h>

struct clap_jack_host
{
  /* clap */
  struct clap_host    host;
  struct clap_plugin *plugin;
  void               *library_handle;

  /* host */
  uint32_t sample_rate;
  uint64_t steady_time;

  /* jack */
  jack_client_t *client;
  jack_port_t   *input_ports[2];
  jack_port_t   *output_ports[2];
  jack_port_t   *midi_in;
};

static void deinitialize(struct clap_jack_host *app);

static void host_events(struct clap_host   *host,
                        struct clap_plugin *plugin,
                        struct clap_event  *events)
{
}

static uint64_t host_steady_time(struct clap_host *host)
{
  return 0;
}

static void *host_extension(struct clap_host *host, const char *extension_id)
{
  return NULL;
}

static void host_log(struct clap_host       *host,
                     struct clap_plugin     *plugin,
                     enum clap_log_severity  severity,
                     const char             *msg)
{
  const char *severities[] = {
    "debug",
    "info",
    "warning",
    "error",
    "fatal",
  };

  fprintf(stdout, "[%s] %s\n", severities[severity], msg);
}

static uint32_t host_attribute(struct clap_host *host,
                               const char       *attr,
                               char             *buffer,
                               uint32_t          size)
{
  return 0;
}

int process(jack_nframes_t nframes, void *arg)
{
  struct clap_jack_host *app = arg;

  /* get jack ports */
  jack_default_audio_sample_t *in[2], *out[2];
  for (int i = 0; i < 2; ++i) {
    in[i]  = jack_port_get_buffer(app->input_ports[i], nframes);
    out[i] = jack_port_get_buffer(app->output_ports[i], nframes);
  }
  void *midi_in_buf = jack_port_get_buffer(app->midi_in, nframes);
  uint32_t midi_in_count = jack_midi_get_event_count(midi_in_buf);

  struct clap_process p;
  p.input  = in;
  p.output = out;
  p.samples_count = nframes;
  p.steady_time = app->steady_time;
  // XXX add time info

  /* convert midi events */
  p.events = NULL;
  struct clap_event *last_event = NULL;
  for (uint32_t i = 0; i < midi_in_count; ++i) {
    jack_midi_event_t midi;
    jack_midi_event_get(&midi, midi_in_buf, i);

    struct clap_event *event = calloc(1, sizeof (*event));
    if (!event)
      break;
    if (last_event)
      last_event->next = event;
    else
      p.events = event;
    last_event = event;

    clap_midi_convert(midi.buffer, midi.size, event);
    event->steady_time = app->steady_time + midi.time;
  }

  /* process */
  app->plugin->process(app->plugin, &p);

  /* release events */
  while (p.events) {
    struct clap_event *next = p.events->next;
    free(p.events);
    p.events = next;
  }

  app->steady_time += nframes;
  return 0;
}

void shutdown(void *arg)
{
  deinitialize(arg);
}

static bool initialize(struct clap_jack_host *app,
                       const char            *plugin_path,
                       uint32_t               plugin_index)
{
  /* jack */
  jack_status_t jack_status;
  app->client = jack_client_open("clap-host", JackNullOption, &jack_status, NULL);
  if (app->client == NULL) {
    fprintf(stderr, "jack_client_open() failed, status: %d\n", jack_status);
    return false;
  }

  jack_set_process_callback(app->client, process, app);
  jack_on_shutdown(app->client, shutdown, app);
  app->input_ports[0] = jack_port_register(app->client, "input left",
                                           JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsInput, 0);
  app->input_ports[1] = jack_port_register(app->client, "input right",
                                           JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsInput, 0);
  app->output_ports[0] = jack_port_register(app->client, "output left",
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsOutput, 0);
  app->output_ports[1] = jack_port_register(app->client, "output right",
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsOutput, 0);
  app->midi_in = jack_port_register(app->client, "midi in", JACK_DEFAULT_MIDI_TYPE,
                                    JackPortIsInput, 0);

  printf("engine sample rate: %"PRIu32"\n", jack_get_sample_rate(app->client));

  /* host initialization */
  app->host.clap_version  = CLAP_VERSION;
  app->host.events        = host_events;
  app->host.steady_time   = host_steady_time;
  app->host.extension     = host_extension;
  app->host.get_attribute = host_attribute;
  app->host.log           = host_log;
  app->steady_time = 0;
  app->sample_rate = jack_get_sample_rate(app->client);

  /* plugin initialization */
  app->library_handle = dlopen(plugin_path, RTLD_NOW | RTLD_LOCAL);
  if (!app->library_handle) {
    fprintf(stderr, "failed to load %s: %s\n", plugin_path, dlerror());
    goto fail_jack;
  }

  union {
    void          *ptr;
    clap_create_f  clap_create;
  } symbol;

  symbol.ptr = dlsym(app->library_handle, "clap_create");
  if (!symbol.ptr) {
    fprintf(stderr, "symbol not found: clap_create\n");
    goto fail_dlclose;
  }

  uint32_t plugin_count;
  app->plugin = symbol.clap_create(plugin_index, &app->host, app->sample_rate, &plugin_count);
  if (!app->plugin) {
    fprintf(stderr, "failed to create plugin\n");
    goto fail_dlclose;
  }

  return true;

fail_dlclose:
  dlclose(app->library_handle);
fail_jack:
  jack_client_close(app->client);
  return false;
}

static void deinitialize(struct clap_jack_host *app)
{
  /* clap */
  app->plugin->destroy(app->plugin);
  dlclose(app->library_handle);

  /* jack */
  jack_client_close(app->client);
}

int main(int argc, char **argv)
{
  struct clap_jack_host app;

  if (argc != 3) {
    fprintf(stderr, "usage: %s plugin.so index\n", argv[0]);
    return 2;
  }

  if (!initialize(&app, argv[1], atoi(argv[2])))
    return 1;

  if (!app.plugin->activate(app.plugin)) {
    fprintf(stderr, "can't activate the plugin\n");
    return 1;
  }

  struct clap_plugin_gui *gui = app.plugin->extension(app.plugin, CLAP_EXT_GUI);
  if (gui)
    gui->open(app.plugin);

  if (jack_activate(app.client)) {
    fprintf(stderr, "can't activate jack.\n");
    return 1;
  }

  // wait until application closes
  sleep(-1);

  deinitialize(&app);
  return 0;
}
