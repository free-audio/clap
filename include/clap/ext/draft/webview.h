#pragma once

static CLAP_CONSTEXPR const char CLAP_EXT_WEBVIEW[] = "clap.webview/1";

// clap.gui API constant. The pointer in clap_window must be NULL, but sizing methods are useful.
// uses logical size, don't call clap_plugin_gui->set_scale()
static const CLAP_CONSTEXPR char CLAP_WINDOW_API_WEBVIEW[] = "webview";

#ifdef __cplusplus
extern "C" {
#endif

/// @page Webview
///
/// This extension enables the plugin to provide the start-page for a webview UI, and exchange
/// messages back and forth.
///
/// Messages are received in the webview using a standard MessageEvent, with the data in an
/// ArrayBuffer. They are posted back to the plugin using window.parent.postMessage(), with the
/// data in an ArrayBuffer or TypedArray.

typedef struct clap_plugin_webview {
   // Returns the URL for the webview's initial navigation, as a null-terminated UTF-8 string.
   // This must be called at least once before any messages are sent (or accepted by the host).
   // If this URL is relative, it is resolved relative to the plugin (bundle) resource directory.
   // The host may assume that no resources outside of that directory are used, and may use any
   // protocol to provide this content, following HTTP-like relative URL resolution. The page must
   // not assume that the root path of the domain is the root of the bundle. The URL may also be
   // absolute, including a `file://` URL.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *provide_starting_uri)(const clap_plugin_t *plugin,
                             char                *out_buffer,
                             uint32_t            out_buffer_capacity);

   // Receives a single message from the webview, which must be open and ready to receive replies.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *receive)(const clap_plugin_t *plugin, const void *buffer, uint32_t size);

} clap_plugin_webview_t;

typedef struct clap_host_webview {
   // Sends a single message to the webview.
   // Returns true on success. It must fail (false) if the webview is not open.
   // [main-thread]
   bool(CLAP_ABI *send)(const clap_host_t *host, const void *buffer, uint32_t size);

} clap_host_webview_t;

#ifdef __cplusplus
}
#endif
