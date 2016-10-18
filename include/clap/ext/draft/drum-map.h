#ifndef CLAP_EXT_DRUM_MAP_H
# define CLAP_EXT_DRUM_MAP_H

# include "../../clap.h"

# define CLAP_EXT_DRUM_MAP "clap/drum-map"

struct clap_drum_map_key_info
{
  char   name[CLAP_NAME_SIZE];
  int8_t key;
  int8_t channel; // -1 for every channels
};

struct clap_plugin_drum_map
{
  /* Get the number of keys which have a drum map info on
   * the given port_id.
   * [thread-safe] */
  int32_t (*get_key_count)(struct clap_plugin *plugin,
                           int32_t             port_id);

  /* Loads the info for the given event port and key index.
   * Returns true on success, false if the info could not be
   * loaded.
   * [thread-safe] */
  bool (*get_key_info)(struct clap_plugin            *plugin,
                       int32_t                        port_id,
                       int32_t                        index,
                       struct clap_drum_map_key_info *info);
};

#endif /* !DRUM_MAP_H */
