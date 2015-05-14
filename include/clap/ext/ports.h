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
  int32_t inputs_count;
  int32_t outputs_count;
};

/* The audio ports configuration has to be done while the plugin is
 * deactivated. */
struct clap_plugin_ports
{
  /* Returns the number of available configurations */
  int32_t (*get_configs_count)(struct clap_plugin *plugin);
  bool (*get_config)(struct clap_plugin       *plugin,
                     int32_t                  config_index,
                     struct clap_ports_config *config);
  bool (*get_info)(struct clap_plugin    *plugin,
                   int32_t               config_index,
                   int32_t               port_index,
                   struct clap_port_info *port);
  bool (*set_config)(struct clap_plugin *plugin,
                     int32_t            config_index);
  bool (*set_repeat)(struct clap_plugin  *plugin,
                     int32_t             port_index,
                     int32_t             count);
};

#endif /* !CLAP_EXT_PORT_H */
