#ifndef CLAP_EXT_EMBED_H
# define CLAP_EXT_EMBED_H

# include "../clap.h"

# define CLAP_EXT_EMBED "clap/embed"

struct clap_host_embed
{
  /* Request the host to resize the client area to width, height.
   * Return true on success, false otherwise. */
  bool (*resize)(struct clap_host *host, uint32_t width, uint32_t height);
};

#endif /* !CLAP_EXT_EMBED_H */
