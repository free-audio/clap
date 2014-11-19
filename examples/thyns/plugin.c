#include <stdlib.h>
#include <stdio.h>
#include <clap/clap.h>
#include <clap/clap-ports.h>

#include "thyns.h"

struct thyns_plugin
{
  struct thyns              thyns;
  struct clap_plugin        plugin;
  struct clap_plugin_ports  ports;
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

uint32_t
thyns_plugin_get_ports_configs_count(struct clap_plugin *plugin)
{
  return 1;
}

bool
thyns_plugin_get_ports_config(struct clap_plugin       *plugin,
                              uint32_t                  config_index,
                              struct clap_ports_config *config)
{
  switch (config_index) {
  case 0:
    snprintf(config->name, sizeof (config->name), "mono");
    config->inputs_count = 0;
    config->outputs_count = 1;
    return true;

  default:
    return false;
  }
}

bool
thyns_plugin_get_port_info(struct clap_plugin    *plugin,
                           uint32_t               config_index,
                           uint32_t               port_index,
                           struct clap_port_info *port)
{
  switch (config_index) {
  case 0:
    switch (port_index) {
    case 0:
      snprintf(port->name, sizeof (port->name), "out");
      port->type          = CLAP_PORT_MONO;
      port->role          = CLAP_PORT_INOUT;
      port->is_repeatable = false;
      return true;;
    }
    return false;

  default:
    return false;
  }
  return true;
}

bool
thyns_plugin_set_ports_config(struct clap_plugin *plugin,
                              uint32_t            config_index)
{
  switch (config_index) {
  case 0:
    return true;

  default:
    return false;
  }
}

enum clap_process_status
thyns_plugin_process(struct clap_plugin  *plugin,
                     struct clap_process *process)
{
  return thyns_process(plugin->plugin_data, process);
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

  // initialize ports extension
  p->ports.get_ports_configs_count = thyns_plugin_get_ports_configs_count;
  p->ports.get_ports_config = thyns_plugin_get_ports_config;
  p->ports.get_port_info = thyns_plugin_get_port_info;
  p->ports.set_ports_config = thyns_plugin_set_ports_config;
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
