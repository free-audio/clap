#pragma once

#include "../plugin.h"

/// @page GUI
///
/// This extension defines how the plugin will present its GUI.
///
/// There are two approaches:
/// 1. the plugin creates a window and embeds it into the host's window
/// 2. the plugin creates a floating window
///
/// Embedding the window gives more control to the host, and feels more integrated.
/// Floating window are sometimes the only option due to technical limitations.
///
/// Showing the GUI works as follow:
/// 1. clap_plugin_gui->create(), allocates gui resources
/// 2. clap_plugin_gui->set_scale()
/// 3. clap_plugin_gui->get_size(), gets initial size
/// 4. clap_plugin_gui->embed()
/// 5. clap_plugin_gui->show()
/// 6. clap_plugin_gui->hide()/show() ...
/// 7. clap_plugin_gui->destroy() when done with the gui
///
/// For floating windows, simply don't call clap_plugin_gui_embed() and maybe call
/// clap_plugin_gui->suggest_title(), clap_plugin_gui->set_transient()
///
/// Resizing the window (initiated by the plugin):
/// 1. Plugins calls clap_host_gui->request_resize()
/// 2. If the host returns true the new size is accepted,
///    the host doesn't have to call clap_plugin_gui->set_size().
///    If the host returns false, the new size is rejected.
///
/// Resizing the window (drag):
/// 1. Only possible if clap_plugin_gui->can_resize() returns true
/// 2. Mouse drag -> new_size
/// 3. clap_plugin_gui->compute_size_to_fit(new_size) -> working_size
/// 4. clap_plugin_gui->set_size(working_size)

static CLAP_CONSTEXPR const char CLAP_EXT_GUI[] = "clap.gui";

// Known windowing API
enum {
   CLAP_GUI_API_WIN32,
   CLAP_GUI_API_X11,
   CLAP_GUI_API_WAYLAND,
   CLAP_GUI_API_COCOA, // uses logical size
   CLAP_GUI_API_FLOATING,
};

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_gui_window_x11 {
   const char   *display;
   unsigned long window;
} clap_gui_window_x11_t;

typedef struct clap_gui_window_cocoa {
   void *nsView;
} clap_gui_window_cocoa_t;

typedef void *clap_hwnd;
typedef struct clap_gui_window_win32 {
   clap_hwnd window;
} clap_gui_window_win32_t;

// Represent a window reference.
// api is one of CLAP_GUI_API_XXX
// specific has to be casted to the corresponding clap_gui_window_xxx.
typedef struct clap_gui_window {
   uint32_t    api;
   const void *specific;
} clap_gui_window_t;

// Size (width, height) is in pixels; the corresponding windowing system extension is
// responsible to define if it is physical pixels or logical pixels.
typedef struct clap_plugin_gui {
   // Create and allocate all resources necessary for the gui, and for the given windowing API.
   // After this call, the GUI is ready to be shown but it is not yet visible.
   // [main-thread]
   CLAP_NODISCARD bool (*create)(const clap_plugin_t *plugin, uint32_t api);

   // Free all resources associated with the gui.
   // [main-thread]
   void (*destroy)(const clap_plugin_t *plugin);

   // Set the absolute GUI scaling factor, and override any OS info.
   // If the plugin does not provide this function, then it should work out the scaling factor
   // itself by querying the OS directly.
   //
   // Return false if the plugin can't apply the scaling; true on success.
   // [main-thread,optional]
   CLAP_NODISCARD bool (*set_scale)(const clap_plugin_t *plugin, double scale);

   // Get the current size of the plugin UI.
   // clap_plugin_gui->create() must have been called prior to asking the size.
   // [main-thread]
   CLAP_NODISCARD bool (*get_size)(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);

   // [main-thread]
   CLAP_NODISCARD bool (*can_resize)(const clap_plugin_t *plugin);

   // If the plugin gui is resizable, then the plugin will calculate the closest
   // usable size which fits in the given size.
   // This method does not change the size.
   //
   // [main-thread]
   CLAP_NODISCARD bool (*adjust_size)(const clap_plugin_t *plugin,
                                      uint32_t            *width,
                                      uint32_t            *height);

   // Sets the window size
   // Returns true if the size is supported.
   // [main-thread]
   CLAP_NODISCARD bool (*set_size)(const clap_plugin_t *plugin, uint32_t width, uint32_t height);

   // Attaches the plugin's window to the given parent window.
   // [main-thread]
   CLAP_NODISCARD bool (*attach)(const clap_plugin_t     *plugin,
                                 const clap_gui_window_t *parent_window);

   // In case the window was created with the "free" windowing API.
   // Sets the window to which the plugin's window shall stay above.
   // [main-thread]
   CLAP_NODISCARD bool (*set_transient)(const clap_plugin_t     *plugin,
                                        const clap_gui_window_t *daw_window);

   // Suggests a window title. Only useful when using the "free" windowing API.
   // [main-thread]
   void (*suggest_title)(const clap_plugin_t *plugin, const char *title);

   // Show the window.
   // [main-thread]
   void (*show)(const clap_plugin_t *plugin);

   // Hide the window, this method do not free the resources, it just hides
   // the window content. Yet it maybe a good idea to stop painting timers.
   // [main-thread]
   void (*hide)(const clap_plugin_t *plugin);
} clap_plugin_gui_t;

typedef struct clap_host_gui {
   /* Request the host to resize the client area to width, height.
    * Return true if the new size is accepted, false otherwise.
    * The host doesn't have to call set_size().
    * [main-thread] */
   CLAP_NODISCARD bool (*request_resize)(const clap_host_t *host, uint32_t width, uint32_t height);

   /* Request the host to show the plugin gui.
    * Return true on success, false otherwise.
    * [main-thread] */
   CLAP_NODISCARD bool (*request_show)(const clap_host_t *host);

   /* Request the host to hide the plugin gui.
    * Return true on success, false otherwise.
    * [main-thread] */
   CLAP_NODISCARD bool (*request_hide)(const clap_host_t *host);
} clap_host_gui_t;

#ifdef __cplusplus
}
#endif
