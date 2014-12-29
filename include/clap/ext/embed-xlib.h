#ifndef CLAP_EXT_EMBED_XLIB_H
# define CLAP_EXT_EMBED_XLIB_H

# include <X11/Xlib.h>

# include "../clap.h"
# include "embed.h"

# define CLAP_EXT_EMBED_XLIB "clap/embed/xlib"

struct clap_plugin_embed_xlib
{
  void (*get_size)(uint32_t *width, uint32_t *height);

  /* Note for the client, you can get a Display* by calling
   * XOpenDisplay(display_name).
   *
   * Note for the host, the display_name can be retrieved from your own
   * display->display_name. */
  bool (*attach)(struct clap_plugin *plugin,
                 Window              window,
                 const char         *display_name);
  bool (*detach)(struct clap_plugin *plugin);
};

#endif /* !CLAP_EMBED_XLIB_H */
