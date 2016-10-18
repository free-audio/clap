#ifndef CLAP_EXT_LOCALE_H
# define CLAP_EXT_LOCALE_H

# include "../../clap.h"

# define CLAP_EXT_LOCALE "clap/locale"

struct clap_plugin_locale
{
  /* Sets the locale to use.
   * [thread-safe] */
  bool (*set_locale)(struct clap_plugin *plugin, const char *locale);
};

#endif /* !CLAP_EXT_LOCALE_H */
