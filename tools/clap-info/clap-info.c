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

    char buffer[256];

#define print_attr(Attr)                                                \
    do {                                                                \
      plugin->get_attribute(                                            \
        plugin, CLAP_ATTR_##Attr, buffer, sizeof (buffer));             \
      fprintf(stdout, " %s: %s\n", CLAP_ATTR_##Attr, buffer);           \
    } while (0)

    print_attr(ID);
    print_attr(NAME);
    print_attr(DESCRIPTION);
    print_attr(MANUFACTURER);
    print_attr(VERSION);
    print_attr(URL);
    print_attr(SUPPORT);
    print_attr(LICENSE);
    print_attr(CATEGORIES);
    print_attr(TYPE);
    print_attr(CHUNK_SIZE);
    print_attr(LATENCY);
    print_attr(SUPPORTS_TUNING);

#undef print_attr

    // destroy the plugin
    plugin->destroy(plugin);
  }

  dlclose(handle);

  return 0;
}
