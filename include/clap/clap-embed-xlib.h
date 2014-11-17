#ifndef CLAP_EMBED_XLIB_H
# define CLAP_EMBED_XLIB_H

# include <X11/Xlib.h>

# include "clap.h"
# include "clap-embed.h"

# define CLAP_EXT_EMBED_XLIB "clap/embed/xlib"

struct clap_plugin_embed_xlib
{
  void (*get_size)(uint32_t *width, uint32_t *height);

  /* the display_name can be retrieved from your own
   * display->display_name. */
  bool (*embed)(struct clap_plugin *plugin,
                Window              window,
                const char         *display_name);
  bool (*disembed)(struct clap_plugin *plugin);
};

#endif /* !CLAP_EMBED_XLIB_H */
