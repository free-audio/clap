#pragma once

#include "../plugin.h"
#include "../stream.h"

/// @page state2 extension
/// @brief extended state handling
///
/// Main idea:
///
/// Sometimes it is desirable to add additional information about a plugin state
/// depending on context. The extension is designed to be a full replacement 
/// of the ext-state extensions of CLAP.
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
/// Scenarios for the save() function:
/// 
/// - Context `clap_state_context_preset`: The plugin stores all sound settings into the stream
///   and would NOT include any project specific settings (like the hardware's id)
/// - Context `clap_state_context_project`: The plugin stores all aound settings including
///   the project specific settings (like the hardware's id)
/// - Context `clap_state_context_clone`: The plugin stores all relevant settings that allow
///   a new instance of the plugin to be presented as 'duplicate' effectively, like
///   using the next index of an enumeration, a channel etc.
/// 
/// Scenarios for the load() function:
/// 
/// - Context `clap_state_context_preset`: The plugin restores all sound settings from the stream
///   and would IGNORE any project specific settings (like the hardware's id) which possibly
///   is in the stream.
/// - Context `clap_state_context_project`: The plugin restores all aound settings including
///   the project specific settings (like the hardware's id). If no project specific settings
///   are in the stream, those should go to default
/// - Context `clap_state_context_clone`: The plugin restores all relevant settings that allow
///   this instance of the plugin to be presented as 'duplicate' effectively, but probably
///   changing settings in a useful way (like using the next channel, sidechain or whatever)
/// 
/// The plugin is responsible to ignore data that makes not sense in the given context or provide
/// useful settings in case such a data is not contained in the stream.
/// 
/// The host is responsible to declare the context the state operation in which it is happening.
///
/// @note if the `ext-state2` extension is not available, the fallback is always the `ext-state`
/// extension, so a plugin MUST implement both interfaces and handle the stream of `ext-state` 
/// like `clap_context_state_preset`.
/// 
/// @note if an unknown context index is provided, it should be handled as `context_state_preset`.
///
/// 2022 Timo Kaluza / defiant nerd

static CLAP_CONSTEXPR const char CLAP_EXT_STATE2[] = "clap.state2";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_plugin_state2 {

  // the state context
  typedef enum {
    clap_state_context_preset = 1,
    clap_state_context_project = 2,
    clap_state_context_clone = 3
  } clap_plugin_state_context_t;

  // Saves the plugin state for a given context into stream.
  // Returns true if the state was correctly saved.
  // [main-thread]
  bool (*save)(const clap_plugin_t *plugin, clap_plugin_state_context_t context, const clap_ostream_t *stream);

  // Loads the plugin state from stream in a given context
  // Returns true if the state was correctly restored.
  // [main-thread]
  bool (*load)(const clap_plugin_t  *plugin, clap_plugin_state_context_t context, const clap_istream_t *stream);
} clap_plugin_state2_t;

#ifdef __cplusplus
}
#endif
