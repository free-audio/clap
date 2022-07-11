#pragma once

#include "../../plugin.h"
#include "../../stream.h"

/// @page state-context extension
/// @brief extended state handling
///
/// This extension let the host specify how the plugin state should be loaded.
///
/// Briefly, when loading a preset or duplicating a device, the plugin may want to partially load
/// the state and initialize certain things differently.

#ifdef __cplusplus
extern "C" {
#endif

static CLAP_CONSTEXPR const char CLAP_EXT_STATE_CONTEXT[] = "clap.state-context.draft/1";

enum clap_plugin_state_context_type {
   // suitable for duplicating a plugin instance
   CLAP_STATE_CONTEXT_FOR_DUPLICATE = 1,

   // suitable for loading a state as a preset
   CLAP_STATE_CONTEXT_FOR_PRESET = 2,
};

typedef struct clap_plugin_state_context {
   // Loads the plugin state from stream.
   // Returns true if the state was correctly restored.
   // [main-thread]
   void (*load)(const clap_plugin_t *plugin, const clap_istream_t *stream, uint32_t context_type);
} clap_plugin_state_context_t;

#ifdef __cplusplus
}
#endif
