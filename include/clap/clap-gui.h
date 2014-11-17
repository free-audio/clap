#ifndef CLAP_GUI_H
# define CLAP_GUI_H

# include "clap.h"

# define CLAP_EXT_GUI "clap/gui"

struct clap_plugin_gui
{
  /* gui */
  bool (*open_gui)(struct clap_plugin *plugin);
  void (*close_gui)(struct clap_plugin *plugin);
};

#endif /* !CLAP_GUI_H */
