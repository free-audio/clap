#ifndef CLAP_EXT_EMBED_H
# define CLAP_EXT_EMBED_H

# include "../clap.h"

# define CLAP_EXT_EMBED "clap/embed"

struct clap_host_embed
{
  /* Request the host to resize the client area to width, height.
   * Return true on success, false otherwise.
   * [thread-safe] */
  bool (*resize)(struct clap_host *host, int32_t width, int32_t height);
};

#endif /* !CLAP_EXT_EMBED_H */
