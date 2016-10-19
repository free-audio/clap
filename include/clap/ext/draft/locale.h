#ifndef CLAP_EXT_LOCALE_H
# define CLAP_EXT_LOCALE_H

# ifdef __cplusplus
extern "C" {
# endif

# include "../../clap.h"

# define CLAP_EXT_LOCALE "clap/draft/locale"

struct clap_plugin_locale
{
  /* Sets the locale to use.
   * [thread-safe] */
  bool (*set_locale)(struct clap_plugin *plugin, const char *locale);
};

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_EXT_LOCALE_H */
