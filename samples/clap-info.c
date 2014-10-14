#include <stdio.h>
#include <dlfcn.h>

#include <clap/clap.h>

int main(int argc, char **argv)
{
  struct clap_host host; // XXX initialize host

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

    fprintf(stdout,
            "found plugin:\n"
            " id:       %s\n"
            " name:     %s\n"
            " description: %s\n"
            " manufacturer: %s\n"
            " version: %s\n"
            " url: %s\n",
            plugin->id,
            plugin->name,
            plugin->description,
            plugin->manufacturer,
            plugin->version,
            plugin->url);

    // destroy the plugin
    plugin->destroy(plugin);
  }

  return 0;
}
