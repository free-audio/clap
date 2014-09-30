#include <stdio.h>
#include <dlfcn.h>

#include <clap.h>

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

  for (uint32_t index = 0; index < (uint32_t)-1; ++index) {
    struct clap_plugin *plugin = symbold.clap_create(index, host, 48000);
    if (!plugin)
      break;

    fprintf(stdio,
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
