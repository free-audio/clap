#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_UNDO[] = "clap.undo/2";

#ifdef __cplusplus
extern "C" {
#endif

/// @page Undo
///
/// This extension enables the plugin to merge its undo history with the host.
/// This leads to a single undo history shared by the host and many plugins.
///
/// Calling host->undo() or host->redo() is equivalent to clicking undo/redo within the host's GUI.
///
/// If the plugin implements this interface then its undo and redo should be entirely delegated to
/// the host; clicking in the plugin's UI undo or redo is equivalent to clicking undo or redo in the
/// host's UI.
///
/// Some changes are long running changes, for example a mouse interaction will begin editing some
/// complex data and it may take multiple events and a long duration to complete the change.
/// In such case the plugin will call host->begin_change() to indicate the beginning of a long
/// running change and complete the change by calling host->change_made().
///
/// The host may group changes together:
/// [---------------------------------]
/// ^-T0      ^-T1    ^-T2            ^-T3
/// Here a long running change C0 begin at T0.
/// A instantaneous change C1 at T1, and another one C2 at T2.
/// Then at T3 the long running change is completed.
/// The host will then create a single undo step that will merge all the changes into C0.
///
/// This leads to another important consideration: starting a long running change without
/// terminating is **VERY BAD**, because while a change is running it is impossible to call undo or
/// redo.
///
/// Rationale: multiple designs were considered and this one has the benefit of having a single undo
/// history. This simplifies the host implementation, leading to less bugs, a more robust design
/// and maybe an easier experience for the user because there's a single undo context versus one
/// for the host and one for each plugin instance.

enum clap_undo_context_flags {
   // While the host is within a change, it is impossible to perform undo or redo.
   CLAP_UNDO_IS_WITHIN_CHANGE = 1 << 0,
};

enum clap_undo_delta_properties_flags {
   // If not set, then all clap_undo_delta_properties's attributes become irrelevant.
   // If set, then the plugin will provide deltas in host->change_made().
   CLAP_UNDO_DELTA_PROPERTIES_HAS_DELTA = 1 << 0,

   // If set, then the delta will be reusable in the future as long as the plugin is
   // compatible with the given format_version.
   CLAP_UNDO_DELTA_PROPERTIES_IS_PERSISTENT = 1 << 1,
};

typedef struct clap_undo_delta_properties {
   // Bitmask of clap_undo_delta_properties_flags
   uint64_t flags;

   // This represents the delta format version that the plugin is using.
   uint32_t format_version;
} clap_undo_delta_properties_t;

typedef struct clap_plugin_undo {
   // Asks the plugin the delta properties.
   // [main-thread]
   void(CLAP_ABI *get_delta_properties)(const clap_plugin_t          *plugin,
                                        clap_undo_delta_properties_t *properties);

   // Asks the plugin if it can apply a delta using the given format version.
   // Returns true if it is possible.
   // [main-thread]
   bool(CLAP_ABI *can_use_delta_format_version)(const clap_plugin_t *plugin,
                                                clap_id              format_version);

   // Applies synchronously a delta.
   // Returns true on success.
   //
   // [main-thread]
   bool(CLAP_ABI *apply_delta)(const clap_plugin_t *plugin,
                               clap_id              format_version,
                               const void          *delta,
                               size_t               delta_size);

   // Sets the undo context.
   // flags: bitmask of clap_undo_context_flags values
   // names: null terminated string if an redo/undo step exists, null otherwise.
   // [main-thread]
   void(CLAP_ABI *set_context_info)(const clap_plugin_t *plugin,
                                    uint64_t             flags,
                                    const char          *undo_name,
                                    const char          *redo_name);
} clap_plugin_undo_t;

typedef struct clap_host_undo {
   // Begins a long running change.
   // The plugin must not call this twice: there must be either a call to cancel_change() or
   // change_made() before calling begin_change() again.
   // [main-thread]
   void(CLAP_ABI *begin_change)(const clap_host_t *host);

   // Cancels a long running change.
   // cancel_change() must not be called without a preceding begin_change().
   // [main-thread]
   void(CLAP_ABI *cancel_change)(const clap_host_t *host);

   // Completes an undoable change.
   // At the moment of this function call, plugin_state->save() would include the current change.
   //
   // name: mandatory null terminated string describing the change, this is displayed to the user
   //
   // deltas: optional, they are binary blobs used to perform the undo and redo. When not available
   // the host will save the plugin state and use state->load() to perform undo and redo.
   //
   // Note: the provided delta may be used for incremental state saving and crash recovery. The
   // plugin can indicate a format version id and the validity lifetime for the binary blobs.
   // The host can use these to verify the compatibility before applying the delta.
   // If the plugin is unable to use a delta, a notification should be provided to the user and
   // the crash recovery should perform a best effort job, at least restoring the latest saved state.
   //
   // Special case: for objects with shared and synchronized state, changes shouldn't be reported
   // as the host already knows about it.
   // For example, plugin parameter changes shouldn't produce a call to change_made().
   //
   // [main-thread]
   void(CLAP_ABI *change_made)(const clap_host_t *host,
                               const char        *name,
                               const void        *redo_delta,
                               size_t             redo_delta_size,
                               const void        *undo_delta,
                               size_t             undo_delta_size);

   // Asks the host to perform the next undo step.
   // This operation may be asynchronous.
   // [main-thread]
   void(CLAP_ABI *undo)(const clap_host_t *host);

   // Asks the host to perform the next redo step.
   // This operation may be asynchronous.
   // [main-thread]
   void(CLAP_ABI *redo)(const clap_host_t *host);

   // Subscribes to or unsubscribes from undo context info.
   //
   // This method helps reducing the number of calls the host has to perform when updating
   // the undo context info. Consider a large project with 1000+ plugins, we don't want to
   // call 1000+ times update, while the plugin may only need the context info if its GUI
   // is shown and it wants to display undo/redo info.
   //
   // Initial state is unsubscribed.
   //
   // is_subscribed: set to true to receive context info
   //
   // [main-thread]
   void(CLAP_ABI *set_context_info_subscription)(const clap_host_t *host, bool is_subscribed);
} clap_host_undo_t;

#ifdef __cplusplus
}
#endif
