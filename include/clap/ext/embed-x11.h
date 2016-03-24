#ifndef CLAP_EXT_EMBED_X11_H
# define CLAP_EXT_EMBED_X11_H

# include "../clap.h"
# include "embed.h"

# define CLAP_EXT_EMBED_X11 "clap/embed/x11"

struct clap_plugin_embed_x11
{
  void (*get_size)(struct clap_plugin *plugin,
                   int32_t            *width,
                   int32_t            *height);

  /* Note for the client, you can get a Display* by calling
   * XOpenDisplay(display_name).
   *
   * Note for the host, the display_name can be retrieved from your own
   * display->display_name. */
  bool (*attach)(struct clap_plugin *plugin,
                 const char         *display_name,
                 unsigned long       window);
  bool (*detach)(struct clap_plugin *plugin);
};

#endif /* !CLAP_EMBED_XLIB_H */
