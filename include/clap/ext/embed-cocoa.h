#ifndef CLAP_EXT_EMBED_COCOA_H
# define CLAP_EXT_EMBED_COCOA_H

# ifdef __cplusplus
extern "C" {
# endif

# include "../clap.h"
# include "embed.h"

# define CLAP_EXT_EMBED_COCOA "clap/embed/cocoa"

struct clap_plugin_embed_cocoa
{
  /* Get the size of the plugin UI.
   * [thread-safe] */
  void (*get_size)(struct clap_plugin *plugin,
                   int32_t            *width,
                   int32_t            *height);

  /* Attach the plugin UI.
   * [thread-safe] */
  bool (*attach)(struct clap_plugin *plugin,
                 void               *nsView);

  /* Detach the plugin UI.
   * [thread-safe] */
  bool (*detach)(struct clap_plugin *plugin);
};

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_EXT_EMBED_COCOA_H */
