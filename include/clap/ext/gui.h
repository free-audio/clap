#ifndef CLAP_EXT_GUI_H
# define CLAP_EXT_GUI_H

# include "../clap.h"

# define CLAP_EXT_GUI "clap/gui"

struct clap_plugin_gui
{
  /* gui */
  bool (*open)(struct clap_plugin *plugin);
  void (*close)(struct clap_plugin *plugin);
};

#endif /* !CLAP_EXT_GUI_H */
