#ifndef CLAP_EMBED_WIN32_H
# define CLAP_EMBED_WIN32_H

# include <windows.h>

# include "clap.h"

# define CLAP_EMBED_WIN32 "clap/embed/win32"

struct clap_embed_win32
{
  bool (*embed)(struct clap_plugin *plugin,
                HWND                window);
};

#endif /* !CLAP_EMBED_WIN32_H */
