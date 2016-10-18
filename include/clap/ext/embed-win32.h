#ifndef CLAP_EXT_EMBED_WIN32_H
# define CLAP_EXT_EMBED_WIN32_H

# include <windows.h>

# include "../clap.h"
# include "embed.h"

# define CLAP_EXT_EMBED_WIN32 "clap/embed/win32"

struct clap_plugin_embed_win32
{
  /* Get the size of the plugin UI.
   * [thread-safe] */
  void (*get_size)(struct clap_plugin *plugin,
                   int32_t            *width,
                   int32_t            *height);

  /* Attach the plugin UI.
   * [thread-safe] */
  bool (*attach)(struct clap_plugin *plugin, HWND window);

  /* Detach the plugin UI.
   * [thread-safe] */
  bool (*detach)(struct clap_plugin *plugin);
};

#endif /* !CLAP_EXT_EMBED_WIN32_H */
