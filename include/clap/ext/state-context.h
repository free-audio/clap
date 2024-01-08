#pragma once

#include "../plugin.h"
#include "../stream.h"

/// @page state-context extension
/// @brief extended state handling
///
/// This extension lets the host save and load the plugin state with different semantics depending
/// on the context.
///
/// Briefly, when loading a preset or duplicating a device, the plugin may want to partially load
/// the state and initialize certain things differently, like handling limited resources or fixed
/// connections to external hardware resources.
///
/// Save and Load operations may have a different context.
/// All three operations should be equivalent:
/// 1. clap_plugin_state_context.load(clap_plugin_state.save(), CLAP_STATE_CONTEXT_FOR_PRESET)
/// 2. clap_plugin_state.load(clap_plugin_state_context.save(CLAP_STATE_CONTEXT_FOR_PRESET))
/// 3. clap_plugin_state_context.load(
///        clap_plugin_state_context.save(CLAP_STATE_CONTEXT_FOR_PRESET),
///        CLAP_STATE_CONTEXT_FOR_PRESET)
///
/// If in doubt, fallback to clap_plugin_state.
///
/// If the plugin implements CLAP_EXT_STATE_CONTEXT then it is mandatory to also implement
/// CLAP_EXT_STATE.
///
/// It is unspecified which context is equivalent to clap_plugin_state.{save,load}()

#ifdef __cplusplus
extern "C" {
#endif

static CLAP_CONSTEXPR const char CLAP_EXT_STATE_CONTEXT[] = "clap.state-context/2";

enum clap_plugin_state_context_type {
   // suitable for storing and loading a state as a preset
   CLAP_STATE_CONTEXT_FOR_PRESET = 1,

  // suitable for duplicating a plugin instance
   CLAP_STATE_CONTEXT_FOR_DUPLICATE = 2,

   // suitable for storing and loading a state within a project/song
   CLAP_STATE_CONTEXT_FOR_PROJECT = 3,
};

typedef struct clap_plugin_state_context {
   // Saves the plugin state into stream, according to context_type.
   // Returns true if the state was correctly saved.
   //
   // Note that the result may be loaded by both clap_plugin_state.load() and
   // clap_plugin_state_context.load().
   // [main-thread]
   bool(CLAP_ABI *save)(const clap_plugin_t  *plugin,
                        const clap_ostream_t *stream,
                        uint32_t              context_type);

   // Loads the plugin state from stream, according to context_type.
   // Returns true if the state was correctly restored.
   //
   // Note that the state may have been saved by clap_plugin_state.save() or
   // clap_plugin_state_context.save() with a different context_type.
   // [main-thread]
   bool(CLAP_ABI *load)(const clap_plugin_t  *plugin,
                        const clap_istream_t *stream,
                        uint32_t              context_type);
} clap_plugin_state_context_t;

#ifdef __cplusplus
}
#endif
