#ifndef CLAP_EMBED_H
# define CLAP_EMBED_H

# include "clap.h"

# define CLAP_EMBED "clap/embed"

struct clap_host_embed
{
  bool (*resize)(struct clap_host *host, uint32_t width, uint32_t height);
};

#endif /* !CLAP_EMBED_H */
