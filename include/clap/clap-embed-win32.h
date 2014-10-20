#ifndef CLAP_EMBED_WIN32_H
# define CLAP_EMBED_WIN32_H

# include <windows.h>

# include "clap.h"
# include "clap-embed.h"

# define CLAP_EMBED_WIN32 "clap/embed/win32"

struct clap_plugin_embed_win32
{
  void (*get_size)(uint32_t *width, uint32_t *height);
  bool (*embed)(struct clap_plugin *plugin, HWND window);
  bool (*disembed)(struct clap_plugin *plugin);
};

#endif /* !CLAP_EMBED_WIN32_H */
