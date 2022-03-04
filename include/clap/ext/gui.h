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
/// 1. clap_plugin_gui->is_api_supported(), check what can work
/// 2. clap_plugin_gui->create(), allocates gui resources
/// 3. if the plugin window is floating
/// 4.    -> clap_plugin_gui->suggest_title()
/// 5. else
/// 6.    -> clap_plugin_gui->set_scale(), if the function pointer is provided by the plugin
/// 7.    -> clap_plugin_gui->get_size(), gets initial size
/// 8. clap_plugin_gui->show()
/// 9. clap_plugin_gui->hide()/show() ...
/// 10. clap_plugin_gui->destroy() when done with the gui
///
/// Resizing the window (initiated by the plugin, if embedded):
/// 1. Plugins calls clap_host_gui->request_resize()
/// 2. If the host returns true the new size is accepted,
///    the host doesn't have to call clap_plugin_gui->set_size().
///    If the host returns false, the new size is rejected.
///
/// Resizing the window (drag, if embedded)):
/// 1. Only possible if clap_plugin_gui->can_resize() returns true
/// 2. Mouse drag -> new_size
/// 3. clap_plugin_gui->adjust_size(new_size) -> working_size
/// 4. clap_plugin_gui->set_size(working_size)

static CLAP_CONSTEXPR const char CLAP_EXT_GUI[] = "clap.gui";

// If your windowing API is not listed here, please open an issue and we'll figure it out.
// https://github.com/free-audio/clap/issues/new

// uses physical size
// embed using https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setparent
static const CLAP_CONSTEXPR char CLAP_GUI_API_WIN32[] = "win32";

// uses logical size
static const CLAP_CONSTEXPR char CLAP_GUI_API_COCOA[] = "cocoa";

// uses physical size
// embed using https://specifications.freedesktop.org/xembed-spec/xembed-spec-latest.html
static const CLAP_CONSTEXPR char CLAP_GUI_API_X11[] = "x11";

// uses physical size
// embed is currently not supported, use floating windows
static const CLAP_CONSTEXPR char CLAP_GUI_API_WAYLAND[] = "wayland";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_window_x11 {
   const char   *display;
   unsigned long window;
} clap_window_x11_t;

typedef struct clap_window_cocoa {
   void *nsView;
} clap_window_cocoa_t;

typedef void *clap_hwnd;
typedef struct clap_window_win32 {
   clap_hwnd window;
} clap_window_win32_t;

// Represent a window reference.
// api is one of CLAP_GUI_API_XXX
// specific has to be casted to the corresponding clap_gui_window_xxx.
typedef struct clap_window {
   const char *api;
   union {
      clap_window_cocoa_t cocoa;
      clap_window_x11_t   x11;
      clap_window_win32_t win32;
      void               *ptr; // for anything defined outside of clap
   };
} clap_window_t;

// Size (width, height) is in pixels; the corresponding windowing system extension is
// responsible to define if it is physical pixels or logical pixels.
typedef struct clap_plugin_gui {
   // Returns true if the requested gui api is supported
   // [main-thread]
   bool (*is_api_supported)(const clap_plugin_t *plugin, const char *api, bool is_floating);

   // Create and allocate all resources necessary for the gui.
   //
   // If is_floating is true, then the window will not be managed by the host. The plugin
   // can set its window to stays above the parent window.
   //
   // If is_floating is false, then the plugin has to embbed its window into the parent window.
   //
   // After this call, the GUI is ready to be shown but it is not yet visible.
   //
   // [main-thread]
   bool (*create)(const clap_plugin_t *plugin, const clap_window_t *parentWindow, bool is_floating);

   // Free all resources associated with the gui.
   // [main-thread]
   void (*destroy)(const clap_plugin_t *plugin);

   // Set the absolute GUI scaling factor, and override any OS info.
   // If the plugin does not provide this function, then it should work out the scaling factor
   // itself by querying the OS directly.
   //
   // Return false if the plugin can't apply the scaling; true on success.
   // [main-thread,optional]
   bool (*set_scale)(const clap_plugin_t *plugin, double scale);

   // Get the current size of the plugin UI.
   // clap_plugin_gui->create() must have been called prior to asking the size.
   // [main-thread]
   bool (*get_size)(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);

   // Returns true if the window is resizeable (mouse drag).
   // Only for embedded windows.
   // [main-thread]
   bool (*can_resize)(const clap_plugin_t *plugin);

   // If the plugin gui is resizable, then the plugin will calculate the closest
   // usable size which fits in the given size.
   // This method does not change the size.
   //
   // Only for embedded windows.
   // [main-thread]
   bool (*adjust_size)(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);

   // Sets the window size. Only for embedded windows.
   // [main-thread]
   bool (*set_size)(const clap_plugin_t *plugin, uint32_t width, uint32_t height);

   // Suggests a window title. Only for floating windows.
   // [main-thread]
   void (*suggest_title)(const clap_plugin_t *plugin, const char *title);

   // Show the window.
   // [main-thread]
   bool (*show)(const clap_plugin_t *plugin);

   // Hide the window, this method do not free the resources, it just hides
   // the window content. Yet it maybe a good idea to stop painting timers.
   // [main-thread]
   bool (*hide)(const clap_plugin_t *plugin);
} clap_plugin_gui_t;

typedef struct clap_host_gui {
   /* Request the host to resize the client area to width, height.
    * Return true if the new size is accepted, false otherwise.
    * The host doesn't have to call set_size().
    * [main-thread] */
   bool (*request_resize)(const clap_host_t *host, uint32_t width, uint32_t height);

   /* Request the host to show the plugin gui.
    * Return true on success, false otherwise.
    * [main-thread] */
   bool (*request_show)(const clap_host_t *host);

   /* Request the host to hide the plugin gui.
    * Return true on success, false otherwise.
    * [main-thread] */
   bool (*request_hide)(const clap_host_t *host);
} clap_host_gui_t;

#ifdef __cplusplus
}
#endif
