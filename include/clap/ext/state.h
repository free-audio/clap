#ifndef CLAP_EXT_STATE_H
# define CLAP_EXT_STATE_H

# include "../clap.h"

# define CLAP_EXT_STATE "clap/state"

# ifdef __cplusplus
extern "C" {
# endif

struct clap_plugin_state
{
  /* The plugin has to allocate and save its state into *buffer.
   * The plugin is also responsible to free the buffer on the
   * next call to save() or when the plugin is destroyed.
   * [audio-thread] */
  bool (*save)(struct clap_plugin *plugin, void **buffer, int32_t *size);

  /* [audio-thread] */
  bool (*restore)(struct clap_plugin *plugin, const void *buffer, int32_t size);
};

struct clap_host_state
{
  /* Tell the host that the plugin state has changed.
   * [thread-safe] */
  void (*set_dirty)(struct clap_host   *host,
                    struct clap_plugin *plugin);
};

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_EXT_STATE_H */
