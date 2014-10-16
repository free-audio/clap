#include <stdio.h>
#include <dlfcn.h>

#include <clap/clap.h>

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

static void initialize_host(struct clap_host *host)
{
  host->clap_version = CLAP_VERSION;
  host->events       = host_events;
  host->steady_time  = host_steady_time;
  host->extension    = host_extension;
  snprintf(host->name, sizeof (host->name), "clap-info");
  snprintf(host->manufacturer, sizeof (host->manufacturer), "clap");
  snprintf(host->version, sizeof (host->version), "1.0");
}

int main(int argc, char **argv)
{
  struct clap_host host;
  initialize_host(&host);

  void * handle = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (!handle) {
    fprintf(stderr, "failed to load %s: %s\n", argv[1], dlerror());
    return 1;
  }

  union {
    void          *ptr;
    clap_create_f  clap_create;
  } symbol;

  symbol.ptr = dlsym(handle, "clap_create");
  if (!symbol.ptr) {
    fprintf(stderr, "symbol not found: clap_create\n");
    return 1;
  }

  uint32_t plugin_count = -1;
  for (uint32_t index = 0; index < plugin_count; ++index) {
    struct clap_plugin *plugin = symbol.clap_create(
      index, &host, 48000, &plugin_count);

    if (!plugin) {
      fprintf(stderr, "failed to create plugin index %d\n", index);
      continue;
    }

    fprintf(stdout, " id: %s\n", plugin->id);
    fprintf(stdout, " name: %s\n", plugin->name);
    fprintf(stdout, " description: %s\n", plugin->description);
    fprintf(stdout, " manufacturer: %s\n", plugin->manufacturer);
    fprintf(stdout, " version: %s\n", plugin->version);
    fprintf(stdout, " url: %s\n", plugin->url);
    fprintf(stdout, " support: %s\n", plugin->support);
    fprintf(stdout, " license: %s\n", plugin->license);
    fprintf(stdout, " categories: %s\n", plugin->categories);
    fprintf(stdout, " type:");
    if (plugin->type & CLAP_PLUGIN_INSTRUMENT)
      fprintf(stdout, " instrument");
    if (plugin->type & CLAP_PLUGIN_EFFECT)
      fprintf(stdout, " effect");
    if (plugin->type & CLAP_PLUGIN_EVENT_EFFECT)
      fprintf(stdout, " event_effect");
    if (plugin->type & CLAP_PLUGIN_ANALYZER)
      fprintf(stdout, " analyzer");
    fprintf(stdout, "\n");
    fprintf(stdout, " chunk_size: %d\n", plugin->chunk_size);
    fprintf(stdout, " has_gui: %d\n", plugin->has_gui);
    fprintf(stdout, " supports_tunning: %d\n", plugin->supports_tunning);
    fprintf(stdout, " supports_microtones: %d\n", plugin->supports_microtones);

    // destroy the plugin
    plugin->destroy(plugin);
  }

  return 0;
}
