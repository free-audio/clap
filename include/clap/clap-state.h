#ifndef CLAP_STATE_H
# define CLAP_STATE_H

# include "clap.h"

# define CLAP_EXT_STATE "clap/state"

struct clap_plugin_state
{
  /* The plugin has to allocate and save its state into *buffer.
   * The plugin is also responsible to free the buffer on the
   * next call to save() or when the plugin is destroyed. */
  bool (*save)(struct clap_plugin *plugin, void **buffer, uint32_t *size);
  bool (*restore)(struct clap_plugin *plugin, const void *buffer, uint32_t size);
};

#endif /* !CLAP_STATE_H */
