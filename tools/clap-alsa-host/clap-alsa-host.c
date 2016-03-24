#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <clap/clap.h>
#include <clap/helpers/midi-parser.h>
#include <clap/ext/gui.h>
#include <clap/ext/state.h>

#include <alsa/asoundlib.h>

#define SAMPLES_COUNT 512
#define SAMPLE_RATE 48000

struct clap_alsa_host
{
  /* clap */
  struct clap_host    host;
  struct clap_plugin *plugin;
  void               *library_handle;

  /* host */
  uint32_t sample_rate;
  uint64_t steady_time;

  /* alsa */
  snd_pcm_t *pcm;
  snd_seq_t *seq;
  int        seq_port;

  /* buffers */
  float      out[2][SAMPLES_COUNT];

  /* config */
  const char *state_path;

  /* state */
  bool quit;
};

struct clap_alsa_host *g_app = NULL;

static void deinitialize(struct clap_alsa_host *app);

static void host_events(struct clap_host   *host,
                        struct clap_plugin *plugin,
                        struct clap_event  *events)
{
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

bool convert_seq_event(snd_seq_event_t *seq_ev, struct clap_event *clap_ev)
{
  switch (seq_ev->type)
  {
  case SND_SEQ_EVENT_NOTEON:
    clap_ev->type          = CLAP_EVENT_NOTE_ON;
    clap_ev->note.key      = seq_ev->data.note.note;
    clap_ev->note.velocity = seq_ev->data.note.velocity;
    clap_ev->note.pitch    = clap_midi_pitches[clap_ev->note.key];
    return true;

  case SND_SEQ_EVENT_NOTEOFF:
    clap_ev->type          = CLAP_EVENT_NOTE_OFF;
    clap_ev->note.key      = seq_ev->data.note.note;
    clap_ev->note.velocity = seq_ev->data.note.velocity;
    clap_ev->note.pitch    = clap_midi_pitches[clap_ev->note.key];
    return true;

  default:
    return false;
  }
}

int process(struct clap_alsa_host *app)
{
  float *in[2]  = { NULL,        NULL };
  float *out[2] = { app->out[0], app->out[1] };

  struct clap_process p;
  p.inputs        = in;
  p.outputs       = out;
  p.samples_count = SAMPLES_COUNT;
  p.steady_time   = app->steady_time;
  p.events        = NULL;
  // XXX add time info

  /* convert midi events */
  struct clap_event *last_event = NULL;
  snd_seq_event_t *seq_ev = NULL;
  while (snd_seq_event_input(app->seq, &seq_ev) >= 0) {
    if (!seq_ev)
      break;

    struct clap_event *event = calloc(1, sizeof (*event));
    if (!event)
      break;

    if (convert_seq_event(seq_ev, event))
    {
      if (last_event)
        last_event->next = event;
      else
        p.events = event;
      last_event = event;
      event->steady_time = app->steady_time;// seq_ev->time;
    }
    else
    {
      free(event);
    }

    snd_seq_free_event(seq_ev);
  }

  /* process */
  app->plugin->process(app->plugin, &p);

  void *buffs[2] = { out[0], out[1] };
  int err = snd_pcm_writen(app->pcm, buffs, SAMPLES_COUNT);
  if (err < 0)
    fprintf(stderr, "snd_pcm_writen: %s\n", snd_strerror(err));

  /* release events */
  while (p.events) {
    struct clap_event *next = p.events->next;
    free(p.events);
    p.events = next;
  }

  app->steady_time += SAMPLES_COUNT;
  return 0;
}

void shutdown(void *arg)
{
}

static bool initialize(struct clap_alsa_host *app,
                       const char            *device,
                       const char            *plugin_path,
                       uint32_t               plugin_index)
{
  snd_pcm_hw_params_t *hwparams;
  int                  err;

  app->quit = false;

  /* alsa/pcm initialization */
  err = snd_pcm_open(&app->pcm, device, SND_PCM_STREAM_PLAYBACK, 0);
  if (err < 0) {
    fprintf(stderr, "cannot open %s: %s", device, snd_strerror(err));
    return false;
  }

  err = snd_pcm_set_params(app->pcm, SND_PCM_FORMAT_FLOAT,
                           SND_PCM_ACCESS_RW_NONINTERLEAVED,
                           2, SAMPLE_RATE, true, 512);

  if (err < 0) {
    fprintf(stderr, "failed to set the pcm params: %s\n", snd_strerror(err));
    goto fail_pcm;
  }

  /* alsa/seq initialization */
  err = snd_seq_open(&app->seq, "default", SND_SEQ_OPEN_INPUT, SND_SEQ_NONBLOCK);
  if (err < 0) {
    fprintf(stderr, "failed to open sequencer: %s\n", snd_strerror(err));
    goto fail_pcm;
  }

  err = snd_seq_set_client_name(app->seq, "clap-alsa-host");
  if (err < 0) {
    fprintf(stderr, "snd_seq_set_client_name: %s\n", snd_strerror(err));
    goto fail_seq;
  }

  app->seq_port = snd_seq_create_simple_port(
    app->seq, "port",
    SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ|
    SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
    SND_SEQ_PORT_TYPE_MIDI_GENERIC);
  if (app->seq_port < 0) {
    fprintf(stderr, "snd_seq_create_simple_port: %s\n", snd_strerror(app->seq_port));
    goto fail_seq;
  }

  /* host initialization */
  app->host.clap_version  = CLAP_VERSION;
  app->host.events        = host_events;
  app->host.steady_time   = &app->steady_time;
  app->host.extension     = host_extension;
  app->host.get_attribute = host_attribute;
  app->host.log           = host_log;
  app->steady_time        = 0;
  app->sample_rate        = SAMPLE_RATE;

  /* plugin initialization */
  app->library_handle = dlopen(plugin_path, RTLD_NOW | RTLD_LOCAL);
  if (!app->library_handle) {
    fprintf(stderr, "failed to load %s: %s\n", plugin_path, dlerror());
    goto fail_seq;
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
fail_seq:
  snd_seq_close(app->seq);
fail_pcm:
  snd_pcm_close(app->pcm);
  return false;
}

static void load_state(struct clap_alsa_host *app)
{
  struct stat st;
  struct clap_plugin_state *state = app->plugin->extension(
    app->plugin, CLAP_EXT_STATE);

  if (!state)
    return;

  if (!app->state_path)
    return;

  int fd = open(app->state_path, O_RDONLY);
  if (fd < 0)
    return;

  if (fstat(fd, &st)) {
    close(fd);
    return;
  }

  void *mem = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) {
    close(fd);
    return;
  }

  state->restore(app->plugin, mem, st.st_size);

  munmap(mem, st.st_size);
  close(fd);
}

static void save_state(struct clap_alsa_host *app)
{
  struct clap_plugin_state *state = app->plugin->extension(
    app->plugin, CLAP_EXT_STATE);

  if (!state) {
    fprintf(stdout, "no state extension\n");
    return;
  }

  if (!app->state_path) {
    fprintf(stdout, "no state path\n");
    return;
  }

  void     *buffer = NULL;
  uint32_t  size   = 0;
  if (!state->save(app->plugin, &buffer, &size)) {
    fprintf(stdout, "failed to save the state\n");
    return;
  }

  int fd = open(app->state_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    fprintf(stdout, "open(%s): %m\n", app->state_path);
    return;
  }

  write(fd, buffer, size);
  close(fd);
}

static void deinitialize(struct clap_alsa_host *app)
{
  /* clap */
  save_state(app);
  app->plugin->deactivate(app->plugin);
  app->plugin->destroy(app->plugin);
  dlclose(app->library_handle);

  /* alsa */
  snd_pcm_close(app->pcm);
}

void sig_int(int sig)
{
  g_app->quit = true;
}

int main(int argc, char **argv)
{
  static struct clap_alsa_host app;

  g_app = &app;

  signal(SIGINT, sig_int);

  if (argc < 3) {
    fprintf(stderr, "usage: %s plugin.so index [state]\n", argv[0]);
    return 2;
  }

  if (!initialize(&app, "plug:default", argv[1], atoi(argv[2])))
    return 1;

  if (!app.plugin->activate(app.plugin)) {
    fprintf(stderr, "can't activate the plugin\n");
    return 1;
  }

  if (argc == 4) {
    app.state_path = argv[3];
    load_state(&app);
  } else
    app.state_path = NULL;

  struct clap_plugin_gui *gui = app.plugin->extension(app.plugin, CLAP_EXT_GUI);
  if (gui)
    gui->open(app.plugin);

  while (!app.quit)
    process(&app);

  deinitialize(&app);
  return 0;
}
