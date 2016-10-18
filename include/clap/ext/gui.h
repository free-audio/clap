#ifndef CLAP_EXT_GUI_H
# define CLAP_EXT_GUI_H

# include "../clap.h"

# define CLAP_EXT_GUI "clap/gui"

struct clap_plugin_gui
{
  /* Open the GUI
   * [thread-safe] */
  bool (*open)(struct clap_plugin *plugin);

  /* [thread-safe] */
  void (*hide)(struct clap_plugin *plugin);

  /* [thread-safe] */
  void (*close)(struct clap_plugin *plugin);
};

struct clap_host_gui
{
  /* [thread-safe] */
  void (*gui_opened)(struct clap_host   *host,
                     struct clap_plugin *plugin);

  /* [thread-safe] */
  void (*gui_hidden)(struct clap_host   *host,
                     struct clap_plugin *plugin);

  /* [thread-safe] */
  void (*gui_closed)(struct clap_host   *host,
                     struct clap_plugin *plugin);
};

#endif /* !CLAP_EXT_GUI_H */
