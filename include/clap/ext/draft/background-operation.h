#pragma once

#include "../../plugin.h"

static const char CLAP_EXT_BACKGROUND_OPERATION[] = "clap.background_operation.draft/0";

/*

This extension let the host execute some operations on a background thread.

Some operations can be CPU intensive and require a considerable amount of time, and being force to
run them on the main thread prevent parallelisation.

For example, loading a state or activating a plugin can require a bit of processing.
When loading a large project, with a thousand of plugin instances, it is desirable to be able
to load the plugin state and activate the plugin in a background thread. This can significantly
improve the loading time.

Concurrency isn't a trivial thing, and this extension should only be implemented if the plugin has
expensive tasks to perform during load state or activate.

Implementing this extension implies that plugin->activate() can be called from a [bgop-thread].

Here is how this extension works:

1. Check which operation can be performed in background

        plugin_bgop->is_supported(plugin, CLAP_EXT_STATE);

2. Inform the plugin that the host is about to start a background operation

        const bool can_start = plugin_bgop->about_to_start(plugin);

   Once about_to_start() is called, no more call can be made on the main thread.
   If the background thread couldn't be created then it is allowed to jump to the step 6. immediately.

3. Inform the plugin that we've setup the background thread and are about to start (on the
   background thread)

        plugin_bgop->started(plugin);

4. Perform the background operations

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
