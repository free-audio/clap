#ifndef CLAP_EXT_PORTS_H
# define CLAP_EXT_PORTS_H

# include "../clap.h"

# define CLAP_EXT_PORTS "clap/ports"

enum clap_port_type
{
  CLAP_PORT_MONO     = 0,
  CLAP_PORT_STEREO   = 1,
  CLAP_PORT_SURROUND = 2,
};

enum clap_port_role
{
  CLAP_PORT_INOUT     = 0,
  CLAP_PORT_SIDECHAIN = 1,
};

struct clap_port_info
{
  enum clap_port_type  type;
  enum clap_port_role  role;
  char                 name[CLAP_NAME_SIZE];
  bool                 is_repeatable;
};

struct clap_ports_config
{
  char     name[CLAP_NAME_SIZE];
  uint32_t inputs_count;
  uint32_t outputs_count;
};

struct clap_plugin_ports
{
  /* Audio ports.
   * The port configuration has to be done while the plugin is deactivated. */
  uint32_t (*get_ports_configs_count)(struct clap_plugin *plugin);
  bool (*get_ports_config)(struct clap_plugin       *plugin,
                           uint32_t                  config_index,
                           struct clap_ports_config *config);
  bool (*get_port_info)(struct clap_plugin    *plugin,
                        uint32_t               config_index,
                        uint32_t               port_index,
                        struct clap_port_info *port);
  bool (*set_ports_config)(struct clap_plugin *plugin,
                           uint32_t            config_index);
  bool (*set_port_repeat)(struct clap_plugin  *plugin,
                          uint32_t             port_index,
                          uint32_t             count);
};

#endif /* !CLAP_EXT_PORT_H */
