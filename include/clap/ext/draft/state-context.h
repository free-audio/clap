#pragma once

#include "../../plugin.h"
#include "../../stream.h"

/// @page state-context extension
/// @brief extended state handling
///
/// Main idea:
///
/// Sometimes it is desirable to add additional information about a plugin state
/// depending on context. The extension is designed to be a extension for the
/// ext-state extension of CLAP.
///
/// An example could be a plugin that also interacts with an external hardware.
/// Each instance would be connected to a device that is probably using external connections.
///
/// To restore this information, the hardware's id is being stored in the plugin state stream.
///
/// But if a 'preset' is saved (and restored later via load()) there shouldn't be such an
/// information. This would create presets that are tied to a given hardware id.
///
/// Therefore the host should give a context hint for the operation it executes.
///
/// Scenarios for save() function:
///
/// - Context `CLAP_STATE_CONTEXT_PROJECT`: The plugin stores all aound settings including
///   the project specific settings (like the hardware's id)
/// - Context `CLAP_STATE_CONTEXT_PRESET`: The plugin stores all sound settings into the stream
///   and would NOT include any project specific settings (like the hardware's id)
/// - Context `CLAP_STATE_CONTEXT_CLONE`: The plugin stores all relevant settings that allow
///   a new instance of the plugin to be presented as 'duplicate' effectively, like
///   using the next index of an enumeration, a channel etc.
///
/// Scenarios for the load() function:
///
/// - Context `CLAP_STATE_CONTEXT_PROJECT`: The plugin restores all aound settings including
///   the project specific settings (like the hardware's id). If no project specific settings
///   are in the stream, those should go to default
/// - Context `CLAP_STATE_CONTEXT_PRESET`: The plugin restores all sound settings from the stream
///   and would IGNORE any project specific settings (like the hardware's id) which possibly
///   is in the stream.
/// - Context `CLAP_STATE_CONTEXT_CLONE`: The plugin restores all relevant settings that allow
///   this instance of the plugin to be presented as 'duplicate' effectively, but probably
///   changing settings in a useful way (like using the next channel, sidechain or whatever)
///
/// The plugin is responsible to ignore data that makes not sense in the given context or provide
/// useful settings in case such a data is not contained in the stream or the host did not implement
/// the usage of ext-state-context.
///
/// The host is responsible to declare the context the state operation in which it is happening.
///
/// @note if an unknown context index is provided, it should be handled as
/// `CLAP_STATE_CONTEXT_PROJECT`.
///

#ifdef __cplusplus
extern "C" {
#endif

static CLAP_CONSTEXPR const char CLAP_EXT_STATE_CONTEXT[] = "clap.state-context.draft/0";

enum clap_plugin_state_context_type {
   CLAP_STATE_CONTEXT_PROJECT = 1,
   CLAP_STATE_CONTEXT_PRESET = 2,
   CLAP_STATE_CONTEXT_CLONE = 3
};

typedef struct clap_plugin_state_context {
   // Hosts that use the set_state_context() function should *always* call it directly before
   // ->save() or load(). Plugins that implement the set_state_context() function should
   // keep the last assigned context around, regardless of the frequency of invocations.

   // Assign the context for subsequent calls to ->save() or load() of the
   // clap_plugin_state extension.
   // [main-thread]
   void (*set)(const clap_plugin_t *plugin, uint32_t context);
} clap_plugin_state_context_t;

#ifdef __cplusplus
}
#endif
