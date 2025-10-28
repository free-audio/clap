#pragma once

static CLAP_CONSTEXPR const char CLAP_EXT_WEBVIEW[] = "clap.webview/3";

// clap.gui API constant. The pointer in clap_window must be NULL, but sizing methods are useful.
// This uses logical size, don't call clap_plugin_gui->set_scale()
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
   // This must be called at least once before any messages are sent (or accepted) by the host.
   // Absolute URIs (including `data:`, and `file:` URIs on local systems) are always supported.
   // Relative URIs (with absolute paths) refer to resources provided by .get_resource(), which
   // may be provided to the webview with an arbitrary scheme or URI prefix. The host may also
   // translate `file:` URIs to some other scheme or path root, to limit access scope or handle
   // virtual filesystems. Therefore, when using either relative or `file:` URIs, pages must not
   // assume a particular absolute path, only relative paths between resources.
   // Returns the desired length of the URI, or <= 0 for an error. If this is greater than the
   // capacity, then the result was truncated. If the capacity is 0, `uri` may be a null pointer.
   // [main-thread]
   int32_t(CLAP_ABI *get_uri)(const clap_plugin_t *plugin, char *uri, uint32_t uri_capacity);

   // Provides the media type and data for resources, starting with the URI from get_uri().
   // The path must be absolute (starting with `/`) with any host-defined path prefix removed.
   // Returns true if the media type and entire contents of the resource were provided.
   // [main-thread]
   bool(CLAP_ABI *get_resource)(const clap_plugin_t *plugin, const char *path, char *mime, uint32_t mime_capacity, const clap_ostream_t *stream);

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
