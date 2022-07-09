#pragma once

#include "../../plugin.h"
#include "../../stream.h"

/// @page state-context extension
/// @brief extended state handling
///
/// This extension let the host specify how the plugin state should be saved or loaded
/// by setting a context prior to the save or load operation.
///
/// If unspecified, then the context is `CLAP_STATE_CONTEXT_FULL`.
///
/// Save and Load operations may have a different context.
/// Only the following sequences are specified:
///
/// |  save ctx  |  load ctx  |   result  |
/// +------------+------------+-----------+
/// |       full |       full |      full |
/// |       full |     preset |    preset |
/// |       full |  duplicate | duplicate |
/// |  duplicate |  duplicate | duplicate |
/// |  duplicate |       full | duplicate |
/// |     preset |       full |    preset |
/// |     preset |     preset |    preset |

#ifdef __cplusplus
extern "C" {
#endif

static CLAP_CONSTEXPR const char CLAP_EXT_STATE_CONTEXT[] = "clap.state-context.draft/1";

enum clap_plugin_state_context_type {
   // saves and loads *everything*
   CLAP_STATE_CONTEXT_FULL = 1,

   // suitable for duplicating a plugin instance
   CLAP_STATE_CONTEXT_FOR_DUPLICATE = 2,

   // suitable for saving and loading a preset state
   CLAP_STATE_CONTEXT_FOR_PRESET = 3,
};

typedef struct clap_plugin_state_context {
   // Assign the context for subsequent calls to clap_plugin_state->save() or
   // clap_plugin_state->load() of the clap_plugin_state extension.
   // [main-thread]
   void (*set)(const clap_plugin_t *plugin, uint32_t context_type);
} clap_plugin_state_context_t;

#ifdef __cplusplus
}
#endif
