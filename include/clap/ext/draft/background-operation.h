#pragma once

#include "../../plugin.h"

static const char CLAP_EXT_BACKGROUND_OPERATION[] = "clap.background_operation.draft/0";

/*

This extension let the host execute some operations on a background thread.

Some operations can be CPU intensive and require a considerable amount of time, and being forced to
run them on the main thread prevents parallelisation.

For example, loading a state or activating a plugin can require a bit of processing.
When loading a large project, with a thousand plugin instances, it is desirable to be able
to load the plugin state and activate the plugin in a background thread. This can significantly
improve the loading time.

This design choose to use a clear transition from the main-thread to the background thread,
because it is then clear what to expect and how the inter-action will hapen. The other approach
would be to just let the host call a set of functions at anytime from a random thread which
would force the plugin into a highly defensive implementation regarding multi-threading where it'd
become unclear what will happen when and on which threads, and it creates a thousand of possible
scenarios.

Concurrency isn't a trivial thing, and this extension should only be implemented if the plugin has
expensive tasks to perform during load state or activate.

Implementing this extension implies that plugin->activate() can be called from a [bgop-thread].

Here is how this extension works:

1. Check which operation can be performed in background

        plugin_bgop->is_supported(plugin, CLAP_EXT_STATE);

2. Inform the plugin that the host is about to start a background operation

        const bool can_start = plugin_bgop->about_to_start(plugin);

   Once about_to_start() is called, no more call can be made on the main thread.
   If the background thread couldn't be created then it is allowed to jump to the step 6.
   immediately.

3. Inform the plugin that we've setup the background thread and are about to start (on the
   background thread)

        plugin_bgop->started(plugin);

4. Perform the background operations, the host call any method from supported extensions which are
   marked as [main-thread] on the [bgop-thread].

        plugin_state->load(...)
        ...
        plugin->activate(...)

5. Inform the plugin that we're about to quit the background thread

        plugin_bgop->about_to_finish(plugin);

6. Inform the plugin that we're done and back onto the main thread

        plugin_bgop->finished(plugin);

*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_background_operation {
   // Returns true if an extension can be used in [bgop-thread].
   // [main-thread]
   bool(CLAP_ABI *is_supported)(const clap_plugin_t *plugin, const char *extension_id);

   // The host is about to start a background operation.
   // Return false to cancel the background operation.
   // If the plugin returns true then no further call can be made on the [main-thread] by the plugin
   // and host, except `finished()`.
   //
   // [main-thread]
   bool(CLAP_ABI *about_to_start)(const clap_plugin_t *plugin);

   // The background operation has started.
   // The background thread must remain the same for the whole background operation.
   //
   // [bgop-thread]
   void(CLAP_ABI *started)(const clap_plugin_t *plugin);

   // The background operation is about to finish.
   // [bgop-thread]
   void(CLAP_ABI *about_to_finish)(const clap_plugin_t *plugin);

   // The background operation is now finished.
   // The plugin and host can call each other on the [main-thread] again.
   //
   // [main-thread]
   void(CLAP_ABI *finished)(const clap_plugin_t *plugin);
} clap_plugin_background_operation_t;

#ifdef __cplusplus
}
#endif
