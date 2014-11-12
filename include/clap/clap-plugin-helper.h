#ifndef CLAP_PLUGIN_HELPER_H
# define CLAP_PLUGIN_HELPER_H

# include "clap.h"

/* populate plugin with the default methods */
static inline void
clap_plugin_default(struct clap_plugin *plugin);

# include "clap-plugin-helper.c"

#endif /* !CLAP_PLUGIN_HELPER_H */
