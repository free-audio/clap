#include <stdlib.h>
#include <stdio.h>
#include <clap/clap.h>
#include <clap/clap-ports.h>

#include "thyns.h"

struct thyns_plugin
{
  struct thyns              thyns;
  struct clap_plugin        plugin;
  struct clap_host         *host;
};

void
thyns_plugin_destroy(struct clap_plugin *plugin)
{
  if (!plugin)
    return;

  struct thyns_plugin *p = plugin->plugin_data;
  free(p);
}

uint32_t
thyns_plugin_get_attribute(struct clap_plugin *plugin,
                           const char         *attr,
                           char               *buffer,
                           uint32_t            size)
{
#define attr(Attr, Value)                       \
  do {                                          \
    if (!strcmp(CLAP_ATTR_##Attr, attr)) {      \
      snprintf(buffer, size, "%s", Value);      \
      return sizeof (Value) - 1;                \
    }                                           \
  } while (0)

  attr(ID, "clap/thyns");
  attr(NAME, "Thyns");
  attr(DESCRIPTION, "Clap demo synth");
  attr(VERSION, "0.0.1");
  attr(MANUFACTURER, "Clap");
  attr(URL, "https://github.com/abique/clap");
  attr(SUPPORT, "https://github.com/abique/clap");
  attr(LICENSE, "MIT");
  attr(CATEGORIES, "");
  attr(TYPE, "instrument");
  attr(CHUNK_SIZE, "1");
  attr(SUPPORTS_TUNING, "1");
  attr(LATENCY, "0");

  return 0;

#undef attr
}

enum clap_process_status
thyns_plugin_process(struct clap_plugin  *plugin,
                     struct clap_process *process)
{
  return thyns_process(plugin->plugin_data, process);
}

void *
thyns_plugin_extension(struct clap_plugin *plugin, const char *extension)
{
  return NULL;
}

bool
thyns_plugin_activate(struct clap_plugin *plugin)
{
  return true;
}

void
thyns_plugin_deactivate(struct clap_plugin *plugin)
{
}

struct thyns_plugin *
thyns_plugin_create(struct clap_host *host,
                    uint32_t          sample_rate)
{
  struct thyns_plugin *p = calloc(sizeof (*p), 1);
  if (!p)
    return NULL;

  // initialize the dsp
  thyns_init(&p->thyns, sample_rate);

  // save host pointer
  p->host = host;

  // initialize plugin
  p->plugin.clap_version = CLAP_VERSION;
  p->plugin.destroy = thyns_plugin_destroy;
  p->plugin.plugin_data = p;
  p->plugin.get_attribute = thyns_plugin_get_attribute;
  p->plugin.process = thyns_plugin_process;
  p->plugin.extension = thyns_plugin_extension;
  p->plugin.activate = thyns_plugin_activate;
  p->plugin.deactivate = thyns_plugin_deactivate;

  return p;
}

struct clap_plugin *
clap_create(uint32_t          plugin_index,
            struct clap_host *host,
            uint32_t          sample_rate,
            uint32_t         *plugins_count)
{
  if (plugins_count)
    *plugins_count = 1;

  switch (plugin_index) {
  case 0: {
    struct thyns_plugin *p = thyns_plugin_create(host, sample_rate);
    return p ? &p->plugin : NULL;
  }

  default: return NULL;
  }
}
