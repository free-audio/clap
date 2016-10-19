#ifndef CLAP_EXT_RENDER_H
# define CLAP_EXT_RENDER_H

# include "../clap.h"

# define CLAP_EXT_RENDER "clap/render"

# ifdef __cplusplus
extern "C" {
# endif

enum clap_plugin_render_mode
{
  /* Default setting, used to play "realtime". */
  CLAP_RENDER_REALTIME = 0,

  /* Render setting, used while rendering the song. */
  CLAP_RENDER_OFFLINE  = 1,
};

/* The render extension is used to have different quality settings while
 * playing realtime and while rendering. This feature is useful if your
 * processor is not fast enough to render at maximum quality in realtime.
 *
 * It should be used with care because, if you do all your equalization
 * with realtime settings and you render with offline settings, then
 * you might not get the sound you expected.
 */
struct clap_plugin_render
{
  /* Sets the plugin render mode, while the plugin is deactivated.
   * Returns true on success, false otherwise.
   * On failure the render mode is unchanged.
   * [audio-thread] */
  bool (*set_render_mode)(struct clap_plugin           *plugin,
                          enum clap_plugin_render_mode  mode);

  /* Gets the current rendering mode, can be set anytime.
   * [audio-thread] */
  enum clap_plugin_render_mode (*get_render_mode)(struct clap_plugin *plugin);
};

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_EXT_RENDER_H */
