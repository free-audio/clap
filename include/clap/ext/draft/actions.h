#pragma once

#include "../../plugin.h"
#include "../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_ACTIONS[] = "clap.actions.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

/// @page Actions
///
/// This extension enables the plugin to provide a set of actions.
/// These actions then can be accessed and triggered by the host.
///
/// Some examples for actions:
/// - randomizing step values of a step sequencer
/// - start recording into an plugin-internal audio/note buffer
/// - trigger an retrospective audio/note looper
/// - init current patch
/// - turn off all effects units (for a synthesizer)
/// - trigger a sample-and-hold unit (maybe even per-voice)

enum {
   // Is this action currently available?
   //
   // The host can use this to display corresponding action items as enabled/disabled,
   // like action buttons or menu entries.
   CLAP_ACTION_IS_ENABLED = 1 << 0,

   // When set:
   // - action triggers can be recorded
   // - action triggers can be played back
   //
   // The host can send live user triggers for this action regardless of this flag.
   //
   // If this action affects the internal processing structure of the plugin, ie: max delay, fft
   // size, ... and the plugins needs to re-allocate its working buffers, then it should call
   // host->request_restart(), and perform the change once the plugin is re-activated.
   CLAP_ACTION_IS_AUTOMATABLE = 1 << 1,

   // Does this action support per note automations?
   CLAP_ACTION_IS_AUTOMATABLE_PER_NOTE_ID = 1 << 2,

   // Does this action support per key automations?
   CLAP_ACTION_IS_AUTOMATABLE_PER_KEY = 1 << 3,

   // Does this action support per channel automations?
   CLAP_ACTION_IS_AUTOMATABLE_PER_CHANNEL = 1 << 4,

   // Does this action support per port automations?
   CLAP_ACTION_IS_AUTOMATABLE_PER_PORT = 1 << 5,

   // Any trigger of this action will affect the plugin output and requires to be done via
   // process() if the plugin is active.
   //
   // A simple example would be a sample-and-hold action, triggering it will change the output signal and must be
   // processed.
   CLAP_ACTION_REQUIRES_PROCESS = 1 << 6,
};
typedef uint32_t clap_action_info_flags;

/* This describes an action */
typedef struct clap_action_info {
   // stable action identifier, it must never change.
   clap_id id;

   clap_action_info_flags flags;

   // in analogy to clap_param_info.cookie
   void *cookie;

   // displayable name
   char name[CLAP_NAME_SIZE];

   // the module path containing the action, eg:"sequencers/seq1"
   // '/' will be used as a separator to show a tree like structure.
   char module[CLAP_PATH_SIZE];
} clap_action_info_t;

typedef struct clap_plugin_actions {
   // Returns the number of actions.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Copies the action's info to action_info and returns true on success.
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_plugin_t *plugin,
                            uint32_t index,
                            clap_action_info_t *action_info);

   // Flushes a set of action triggers.
   // This method must not be called concurrently to clap_plugin->process().
   //
   // Note: if the plugin is processing, then the process() call will already achieve the
   // triggers, so a call to flush isn't required, also be aware
   // that the plugin may use the sample offset in process(), while this information would be
   // lost within flush().
   //
   // [active ? audio-thread : main-thread]
   void(CLAP_ABI *flush)(const clap_plugin_t        *plugin,
                         const clap_input_events_t  *in,
                         const clap_output_events_t *out);
} clap_plugin_actions_t;

enum {
   // The action info did change, use this flag for:
   // - name change
   // - module change
   // - is_enabled (flag)
   // New info takes effect immediately.
   CLAP_ACTION_RESCAN_INFO = 1 << 0,

   // Invalidates everything the host knows about actions.
   // It can only be used while the plugin is deactivated.
   // If the plugin is activated use clap_host->restart() and delay any change until the host calls
   // clap_plugin->deactivate().
   //
   // You must use this flag if:
   // - some actions were added or removed.
   // - some actions had critical changes:
   //   - is_per_note (flag)
   //   - is_per_key (flag)
   //   - is_per_channel (flag)
   //   - is_per_port (flag)
   //   - cookie
   CLAP_ACTION_RESCAN_ALL = 1 << 1,
};
typedef uint32_t clap_action_rescan_flags;

enum {
   // Clears all possible references to an action
   CLAP_ACTION_CLEAR_ALL = 1 << 0,

   // Clears all automations to an action
   CLAP_ACTION_CLEAR_AUTOMATIONS = 1 << 1,
};
typedef uint32_t clap_action_clear_flags;

typedef struct clap_host_actions {
   // Rescan the full list of actions according to the flags.
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host, clap_action_rescan_flags flags);

   // Clears references to an action.
   // [main-thread]
   void(CLAP_ABI *clear)(const clap_host_t *host, clap_id action_id, clap_action_clear_flags flags);

   // Request an action flush.
   //
   // The host will then schedule a call to either:
   // - clap_plugin.process()
   // - clap_plugin_actions.flush()
   //
   // This function is always safe to use and should not be called from an [audio-thread] as the
   // plugin would already be within process() or flush().
   //
   // [thread-safe,!audio-thread]
   void(CLAP_ABI *request_flush)(const clap_host_t *host);
} clap_host_actions_t;

#ifdef __cplusplus
}
#endif
