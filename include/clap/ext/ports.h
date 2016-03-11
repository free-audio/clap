#ifndef CLAP_EXT_AUDIO_PORTS_H
# define CLAP_EXT_AUDIO_PORTS_H

# include "../clap.h"

# define CLAP_EXT_AUDIO_PORTS "clap/audio-ports"

enum clap_audio_port_channel_mapping
{
  CLAP_AUDIO_PORT_UNSPECIFIED = 0,
  CLAP_AUDIO_PORT_MONO = 1,

  // left, right
  CLAP_AUDIO_PORT_STEREO = 2,

  // front left, front right, center, low, surround left, surround right
  // surround back left, surround back right
  CLAP_AUDIO_PORT_SURROUND = 3,
};

enum clap_audio_port_role
{
  CLAP_AUDIO_PORT_INOUT      = 0,
  CLAP_AUDIO_PORT_SIDECHAIN  = 1,
  CLAP_AUDIO_PORT_MODULATION = 2,
};

struct clap_audio_port_info
{
  bool                                 is_input;
  int                                  channel_count;
  enum clap_audio_port_channel_mapping channel_mapping;
  enum clap_audio_port_role            role;
  char                                 name[CLAP_NAME_SIZE];

  /* If false, then the port can be connected only once.
   * If true, then the port can be connected multiple times. */
  bool                                 is_repeatable;
};

struct clap_audio_port
{
  float **data;
};

/* The audio ports configuration has to be done while the plugin is
 * deactivated. */
struct clap_plugin_audio_ports
{
  /* number of ports, including inputs and outputs */
  int32_t (*get_count)(struct clap_plugin         *plugin);

  void (*get_info)(struct clap_plugin             *plugin,
                   int32_t                         index,
                   struct clap_port_info          *info);

  int32_t (*connect)(struct clap_plugin           *plugin,
                     int32_t                       index,
                     const struct clap_audio_port *port,
                     const char                   *user_name);

  void (*disconnect)(struct clap_plugin           *plugin,
                     int32_t                       port_id);

  int32_t (*get_latency)(struct clap_plugin       *plugin,
                         int32_t                   port_id);
};

struct clap_host_audio_ports
{
  /* Tell the host that the plugin ports has changed.
   * The host shall */
  void (*changed)(struct clap_host   *host,
                  struct clap_plugin *plugin);

  /* Tell the host that the latency changed. The host should
   * call get_port_latency on each ports. */
  void (*latency_changed)(struct clap_host *host,
                          struct clap_plugin *plugin);
};

#endif /* !CLAP_EXT_PORT_H */
