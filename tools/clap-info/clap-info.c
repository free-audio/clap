#include <stdio.h>
#include <dlfcn.h>

#include <clap/clap.h>
#include <clap/ext/params.h>

static void host_events(struct clap_host   *host,
                        struct clap_plugin *plugin,
                        struct clap_event  *events)
{
}

static void *host_extension(struct clap_host *host, const char *extension_id)
{
  return NULL;
}

static void initialize_host(struct clap_host *host)
{
  static uint64_t steady_time = 0;

  host->clap_version = CLAP_VERSION;
  host->events       = host_events;
  host->steady_time  = &steady_time;
  host->extension    = host_extension;
}

static void print_attr(struct clap_plugin *plugin)
{
  char buffer[256];

#define prt_attr(Attr)                                          \
  do {                                                          \
    int size = plugin->get_attribute(                           \
      plugin, CLAP_ATTR_##Attr, buffer, sizeof (buffer));       \
    if (size > 0)                                               \
      fprintf(stdout, " %s: %s\n", CLAP_ATTR_##Attr, buffer);   \
  } while (0)

  fprintf(stdout, "Attributes:\n");
  prt_attr(ID);
  prt_attr(NAME);
  prt_attr(DESCRIPTION);
  prt_attr(MANUFACTURER);
  prt_attr(VERSION);
  prt_attr(URL);
  prt_attr(SUPPORT);
  prt_attr(LICENSE);
  prt_attr(CATEGORIES);
  prt_attr(TYPE);
  prt_attr(CHUNK_SIZE);
  prt_attr(LATENCY);
  prt_attr(SUPPORTS_TUNING);
  prt_attr(SUPPORTS_IN_PLACE_PROCESSING);
  prt_attr(IS_REMOTE_PROCESSING);

  fprintf(stdout, "-------------------\n");

#undef print_attr
}

static void print_params(struct clap_plugin *plugin)
{
  struct clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);

  if (!params) {
    fprintf(stdout, "no parameter extension\n");
    return;
  }

  uint32_t count = params->count(plugin);
  fprintf(stdout, "parameters count: %d\n", count);

  struct clap_param param;
  for (uint32_t i = 0; i < count; ++i) {
    if (!params->get(plugin, i, &param))
      continue;

    fprintf(stdout, " => {id: %s, name: %s, desc: %s, display: %s, "
            "is_per_note: %d, is_used: %d, is_periodic: %d",
            param.id, param.name, param.desc, param.display,
            param.is_per_note, param.is_used, param.is_periodic);

    switch (param.type) {
    case CLAP_PARAM_FLOAT:
      fprintf(stdout, ", type: float, value: %f, min: %f, max: %f, default: %f",
              param.value.f, param.min.f, param.max.f, param.deflt.f);
      break;

    case CLAP_PARAM_INT:
      fprintf(stdout, ", type: int, value: %d, min: %d, max: %d, default: %d",
              param.value.i, param.min.i, param.max.i, param.deflt.i);
      break;

    case CLAP_PARAM_ENUM:
      fprintf(stdout, ", type: enum, value: %d, min: %d, max: %d, default: %d",
              param.value.i, param.min.i, param.max.i, param.deflt.i);
      break;

    case CLAP_PARAM_BOOL:
      fprintf(stdout, ", type: bool, value: %d, min: %d, max: %d, default: %d",
              param.value.i, param.min.i, param.max.i, param.deflt.i);
      break;

    case CLAP_PARAM_GROUP:
      fprintf(stdout, ", type: group");
      break;
    }

    fprintf(stdout, "}\n");
  }

  fprintf(stdout, "-------------------\n");
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

    print_attr(plugin);
    print_params(plugin);

    // destroy the plugin
    plugin->destroy(plugin);
  }

  dlclose(handle);

  return 0;
}
