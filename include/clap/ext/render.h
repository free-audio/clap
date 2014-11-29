#ifndef CLAP_EXT_RENDER_H
# define CLAP_EXT_RENDER_H

# include "../clap.h"

# define CLAP_EXT_RENDER "clap/render"

enum clap_plugin_render_mode
{
  CLAP_RENDER_REALTIME = 0,
  CLAP_RENDER_OFFLINE  = 1,
};

struct clap_plugin_render
{
  /* Sets the plugin render mode.
   * Returns true on success, false otherwise.
   * On failure the render mode is unchanged.
   */
  bool (*set_render_mode)(struct clap_plugin           *plugin,
                          enum clap_plugin_render_mode  mode);
};

#endif /* !CLAP_EXT_RENDER_H */
