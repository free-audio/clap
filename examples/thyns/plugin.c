#include <stdlib.h>
#include <stdio.h>
#include <clap/clap.h>

#include "thyns.h"

struct thyns_plugin
{
  struct thyns        thyns;
  struct clap_plugin  plugin;
  struct clap_host   *host;
  float               pitch;
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
    if (!strcmp(Attr, attr)) {                  \
      snprintf(buffer, size, "%s", Value);      \
      return sizeof (Value) - 1;                \
    }                                           \
  } while (0)

  attr(CLAP_ATTR_ID, "clap/thyns");
  attr(CLAP_ATTR_NAME, "Thyns");
  attr(CLAP_ATTR_DESCRIPTION, "Clap demo synth");
  attr(CLAP_ATTR_VERSION, "0.0.1");
  attr(CLAP_ATTR_MANUFACTURER, "Clap");
  attr(CLAP_ATTR_URL, "https://github.com/abique/clap");
  attr(CLAP_ATTR_SUPPORT, "https://github.com/abique/clap");
  attr(CLAP_ATTR_LICENSE, "MIT");
  attr(CLAP_ATTR_CATEGORIES, "");
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
      port->stream_id     = 0;
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

bool
thyns_plugin_set_port_repeat(struct clap_plugin  *plugin,
                             uint32_t             port_index,
                             uint32_t             count)
{
  return false;
}

uint32_t
thyns_plugin_get_params_count(struct clap_plugin *plugin)
{
  return 0;
}

bool
thyns_plugin_get_param(struct clap_plugin *plugin,
                       uint32_t            index,
                       struct clap_param  *param)
{
  return false;
}

uint32_t
thyns_plugin_get_presets_count(struct clap_plugin *plugin)
{
  return 0;
}

bool
thyns_plugin_get_preset(struct clap_plugin *plugin,
                        uint32_t            index,
                        struct clap_preset *preset)
{
  return false;
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

void
thyns_plugin_process(struct clap_plugin  *plugin,
                     struct clap_process *process)
{
  thyns_process(plugin->plugin_data, process);
}

bool
thyns_plugin_open_gui(struct clap_plugin *plugin)
{
  return false;
}

void
thyns_plugin_close_gui(struct clap_plugin *plugin)
{
}

bool
thyns_plugin_save(struct clap_plugin *plugin, void **buffer, uint32_t *size)
{
  return false;
}

bool
thyns_plugin_restore(struct clap_plugin *plugin, const void *buffer, uint32_t size)
{
  return false;
}

bool
thyns_plugin_set_locale(struct clap_plugin *plugin, const char *locale)
{
  return false;
}

void *
thyns_plugin_extension(struct clap_plugin *plugin, const char *id)
{
  return NULL;
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
  p->plugin.type = CLAP_PLUGIN_INSTRUMENT;
  p->plugin.chunk_size = 1;
  p->plugin.has_gui = false;
  p->plugin.supports_tuning = true;
  p->plugin.latency = 0;
  p->plugin.get_attribute = thyns_plugin_get_attribute;
  p->plugin.get_ports_configs_count = thyns_plugin_get_ports_configs_count;
  p->plugin.get_ports_config = thyns_plugin_get_ports_config;
  p->plugin.get_port_info = thyns_plugin_get_port_info;
  p->plugin.set_ports_config = thyns_plugin_set_ports_config;
  p->plugin.set_port_repeat = thyns_plugin_set_port_repeat;
  p->plugin.get_params_count = thyns_plugin_get_params_count;
  p->plugin.get_param = thyns_plugin_get_param;
  p->plugin.get_presets_count = thyns_plugin_get_presets_count;
  p->plugin.get_preset = thyns_plugin_get_preset;
  p->plugin.activate = thyns_plugin_activate;
  p->plugin.deactivate = thyns_plugin_deactivate;
  p->plugin.process = thyns_plugin_process;
  p->plugin.open_gui = thyns_plugin_open_gui;
  p->plugin.close_gui = thyns_plugin_close_gui;
  p->plugin.save = thyns_plugin_save;
  p->plugin.restore = thyns_plugin_restore;
  p->plugin.set_locale = thyns_plugin_set_locale;
  p->plugin.extension = thyns_plugin_extension;
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
