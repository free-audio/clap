#ifndef CLAP_EXT_GUI_H
# define CLAP_EXT_GUI_H

# include "../clap.h"

# define CLAP_EXT_GUI "clap/gui"

struct clap_plugin_gui
{
  /* gui */
  bool (*open)(struct clap_plugin *plugin);
  void (*hide)(struct clap_plugin *plugin);
  void (*close)(struct clap_plugin *plugin);
};

struct clap_host_gui
{
  void (*gui_opened)(struct clap_host   *host,
                     struct clap_plugin *plugin);

  void (*gui_hidden)(struct clap_host   *host,
                     struct clap_plugin *plugin);

  void (*gui_closed)(struct clap_host   *host,
                     struct clap_plugin *plugin);
};

#endif /* !CLAP_EXT_GUI_H */
