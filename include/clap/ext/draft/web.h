#pragma once

static CLAP_CONSTEXPR const char CLAP_EXT_WEB[] = "clap.web/1";

#ifdef __cplusplus
extern "C" {
#endif

/// @page Web
///
/// This extension enables the plugin to provide the start-page for a webview UI, and exchange
/// messages back and forth.
///
/// Messages are received in the webview using a standard MessageEvent, with the data in an
/// ArrayBuffer. They are posted back to the plugin using window.parent.postMessage(), with the
/// data in an ArrayBuffer or TypedArray.

typedef struct clap_plugin_web {
   // Returns the URL for the webview's initial navigation, as a null-terminated UTF-8 string.
   // If this URL is relative, it is resolved relative to the plugin (bundle) directory, and
   // cannot be outside it.  The host may use any protocol to serve this content, and the page
   // must not assume that the root of the domain is the root of the bundle. The URL may also be
   // absolute, including a `file://` URL.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *get_start)(const clap_plugin_t *plugin,
                             char                *out_buffer,
                             uint32_t            out_buffer_capacity);

   // Receives a single message from the webview.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *receive)(const clap_plugin_t *plugin, const void *buffer, uint32_t size);

} clap_plugin_web_t;

typedef struct clap_host_web {
   // Checks whether the webview is open (and ready to receive messages)
   // [thread-safe]
   bool(CLAP_ABI *is_open)(const clap_host_t *host);

   // Sends a single message to the webview.
   // It must not allocate or block if called from the audio thread.
   // Returns true on success.
   // [thread-safe]
   bool(CLAP_ABI *send)(const clap_host_t *host, const void *buffer, uint32_t size);

} clap_host_web_t;

#ifdef __cplusplus
}
#endif
