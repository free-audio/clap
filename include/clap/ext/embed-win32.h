#ifndef CLAP_EXT_EMBED_WIN32_H
# define CLAP_EXT_EMBED_WIN32_H

# include <windows.h>

# include "../clap.h"
# include "embed.h"

# define CLAP_EXT_EMBED_WIN32 "clap/embed/win32"

struct clap_plugin_embed_win32
{
  void (*get_size)(int32_t *width, int32_t *height);
  bool (*attach)(struct clap_plugin *plugin, HWND window);
  bool (*detach)(struct clap_plugin *plugin);
};

#endif /* !CLAP_EXT_EMBED_WIN32_H */
