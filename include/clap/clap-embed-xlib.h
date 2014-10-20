#ifndef CLAP_EMBED_XLIB_H
# define CLAP_EMBED_XLIB_H

# include <X11/Xlib.h>

# include "clap.h"

# define CLAP_EMBED_XLIB "clap/embed/xlib"

struct clap_embed_xlib
{
  bool (*embed)(struct clap_plugin *plugin,
                Window              window,
                Display            *display);
};

#endif /* !CLAP_EMBED_XLIB_H */
