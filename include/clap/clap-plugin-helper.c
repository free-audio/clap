#include <assert.h>

void
clap_default_plugin_destroy(struct clap_plugin *plugin)
{
  assert(false && "this function must be implemented");
}

uint32_t
clap_default_plugin_get_attribute(struct clap_plugin *plugin,
                           const char         *attr,
                           char               *buffer,
                           uint32_t            size)
{
  *buffer = '\0';
  return 0;
}

uint32_t
clap_default_plugin_get_ports_configs_count(struct clap_plugin *plugin)
{
  return 0;
}

bool
clap_default_plugin_get_ports_config(struct clap_plugin       *plugin,
                              uint32_t                  config_index,
                              struct clap_ports_config *config)
{
  return false;
}

bool
clap_default_plugin_get_port_info(struct clap_plugin    *plugin,
                                  uint32_t               config_index,
                                  uint32_t               port_index,
                                  struct clap_port_info *port)
{
  return false;
}

bool
clap_default_plugin_set_ports_config(struct clap_plugin *plugin,
                                     uint32_t            config_index)
{
  return false;
}

bool
clap_default_plugin_set_port_repeat(struct clap_plugin  *plugin,
                                    uint32_t             port_index,
                                    uint32_t             count)
{
  return false;
}

uint32_t
clap_default_plugin_get_params_count(struct clap_plugin *plugin)
{
  return 0;
}

bool
clap_default_plugin_get_param(struct clap_plugin *plugin,
                       uint32_t            index,
                       struct clap_param  *param)
{
  return false;
}

uint32_t
clap_default_plugin_get_presets_count(struct clap_plugin *plugin)
{
  return 0;
}

bool
clap_default_plugin_get_preset(struct clap_plugin *plugin,
                               uint32_t            index,
                               struct clap_preset *preset)
{
  return false;
}

bool
clap_default_plugin_activate(struct clap_plugin *plugin)
{
  return true;
}

void
clap_default_plugin_deactivate(struct clap_plugin *plugin)
{
}

void
clap_default_plugin_process(struct clap_plugin  *plugin,
                            struct clap_process *process)
{
}

bool
clap_default_plugin_open_gui(struct clap_plugin *plugin)
{
  return false;
}

void
clap_default_plugin_close_gui(struct clap_plugin *plugin)
{
}

bool
clap_default_plugin_save(struct clap_plugin  *plugin,
                         void               **buffer,
                         uint32_t            *size)
{
  return false;
}

bool
clap_default_plugin_restore(struct clap_plugin *plugin,
                            const void         *buffer,
                            uint32_t            size)
{
  return false;
}

bool
clap_default_plugin_set_locale(struct clap_plugin *plugin, const char *locale)
{
  return false;
}

void *
clap_default_plugin_extension(struct clap_plugin *plugin, const char *id)
{
  return NULL;
}

static inline void
clap_plugin_default(struct clap_plugin *plugin)
{
  // initialize plugin
  plugin->clap_version            = CLAP_VERSION;
  plugin->destroy                 = clap_default_plugin_destroy;
  plugin->plugin_data             = NULL;
  plugin->get_attribute           = clap_default_plugin_get_attribute;
  plugin->get_ports_configs_count = clap_default_plugin_get_ports_configs_count;
  plugin->get_ports_config        = clap_default_plugin_get_ports_config;
  plugin->get_port_info           = clap_default_plugin_get_port_info;
  plugin->set_ports_config        = clap_default_plugin_set_ports_config;
  plugin->set_port_repeat         = clap_default_plugin_set_port_repeat;
  plugin->get_params_count        = clap_default_plugin_get_params_count;
  plugin->get_param               = clap_default_plugin_get_param;
  plugin->get_presets_count       = clap_default_plugin_get_presets_count;
  plugin->get_preset              = clap_default_plugin_get_preset;
  plugin->activate                = clap_default_plugin_activate;
  plugin->deactivate              = clap_default_plugin_deactivate;
  plugin->process                 = clap_default_plugin_process;
  plugin->open_gui                = clap_default_plugin_open_gui;
  plugin->close_gui               = clap_default_plugin_close_gui;
  plugin->save                    = clap_default_plugin_save;
  plugin->restore                 = clap_default_plugin_restore;
  plugin->set_locale              = clap_default_plugin_set_locale;
  plugin->extension               = clap_default_plugin_extension;
}
