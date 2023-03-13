#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"
#include "../../stream.h"

/// @page Undo/Redo
///
/// For each action that causes an incremental change of the plugin state
/// (which includes parameter values and non-parameter state), the plugin provides
/// an undo object to the host.
/// The host then can apply those objects in reverse order in order to
/// undo those actions. Whenever a plugin action is undone (by the host or the plugin),
/// the host receives a redo object which it then can apply in order to redo the action again.
///
/// As opposed to the state extension, undo/redo objects may not contain the
/// whole plugin state but rather only the necessary data for undoing/redoing a
/// single action on top of the current plugin state. Still sometimes it might be the most
/// efficient if an undo/redo object contains the whole plugin state.
///
/// For each plugin-internal state change, undo or redo action, the plugin needs to buffer
/// a pending event object for the host to pull.
/// The host must pull all pending event objects in order to synchronize its undo/redo stack
/// like this:
/// 1. The plugin notifies the host via mark_event_objects_pending.
/// 2. The host calls pull_next_pending_event_object until
///    has_pending_event_object returns false.
///
/// Only if the host undo/redo stacks are in sync, the host can perform an undo/redo on the plugin
/// via apply_event_object.

static const char CLAP_EXT_UNDO_REDO[] = "clap.undo-redo.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum
{
   // The pulled event object represents an internal state change of the plugin.
   // The host must put the event object onto its undo stack.
   CLAP_UNDO_REDO_OBJECT_CHANGE = 0,
   // The pulled event object represents an undo action.
   // The host must put the event object onto its redo stack.
   CLAP_UNDO_REDO_OBJECT_UNDO = 1,
   // The pulled event object represents a redo action.
   // The host must put the event object onto its undo stack.
   CLAP_UNDO_REDO_OBJECT_REDO = 2,
   // The pulled event object represents an internal state change of the plugin
   // which is already undone again by the time the host pulls it.
   // The host must put the event object onto its redo stack.
   // This is for optimization purpose as the host otherwise would have to
   // pull both a change and a complementary undo object simulate an undo with them.
   // In order to minimize the calls of pull_next_pending_event_object, the plugin should
   // compress its pending event object buffer like this:
   // - Buffer before compression: [Change A, Change B, Change C, Undo C, Undo B]
   // - Buffer after compression: [Change A, Undone change C, Undone change B]
   // The order of undone changes B and C is important as the host will first put
   // undone change C on top of its redo stack and then B on top of that.
   CLAP_UNDO_REDO_OBJECT_UNDONE_CHANGE = 3,
};
typedef int32_t clap_undo_redo_object_type;

// Information about a single incremental change event.
// The host can use context and action to embed a message into its own undo/redo
// lists.
typedef struct clap_change_event_description {
   // A brief, human-readable description of the event context, for example:
   //   - "Filter 1 > Cutoff"
   //   - "Param Seq A"
   //   - "Load Preset"
   // It should not contain the name of the plugin.
   char context[CLAP_NAME_SIZE];
   // A brief, human-readable description of what happened, for example:
   //   - "Set to 100%"
   //   - "Randomized step values"
   //   - "'<preset_name>'"
   char action[CLAP_NAME_SIZE];
} clap_change_event_description_t;

typedef struct clap_plugin_undo_redo{
   // Returns true if the plugin has at least one pending undo/redo object not pulled by the
   // host via pull_next_pending_event_object yet.
   // [main-thread]
   bool(CLAP_ABI *has_pending_event_object)(const clap_plugin_t *plugin);

   // Pulls an event object into stream in order to sync the host undo/redo stacks
   // to the plugin-internal undo/redo stacks.
   // For type CLAP_UNDO_REDO_OBJECT_CHANGE and CLAP_UNDO_REDO_OBJECT_UNDONE_CHANGE,
   // - the plugin must provide an event description together with the object.
   // - the host might omit its redo stack
   // For type CLAP_UNDO_REDO_OBJECT_UNDO and CLAP_UNDO_REDO_OBJECT_REDO
   // - description is ignored as it can be safely assumed that the host already knows it.
   // Returns true if the event object was correctly saved.
   // [main-thread]
   bool(CLAP_ABI *pull_next_pending_event_object)(const clap_plugin_t *plugin,
                                                  clap_undo_redo_object_type* type,
                                                  const clap_ostream_t *stream,
                                                  clap_change_event_description_t* description);

   // Performs an undo/redo action by applying an event object from apply_object_stream.
   // In exchange, the plugin returns the complementary redo/undo event object via
   // exchange_object_stream (the 'complementary object').
   // The complementary object must satisfy the following condition:
   //   - Calling clap_plugin_state::load (see state.h) before apply_event_object must return
   //     the same result as calling clap_plugin_state::load after applying the complementary
   //     object via apply_event_object
   // If is_redo is false,
   //   - the host must remove the applied object from its undo stack
   //   - the host must add the complementary object onto its redo stack.
   // otherwise
   //   - the host must remove the applied object from its redo stack
   //   - the host must add the complementary object onto its undo stack.
   // The host probably wants to re-associate the stored clap_change_event_description from the
   // applied object to the complementary object.
   // The plugin must not call mark_event_objects_pending after applying the event object as the
   // host and plugin undo/redo stacks should already be synced via the exchange object.
   // The host must not call this function if there are still event objects pending.
   // Returns true if the change event object was correctly applied.
   // [main-thread && !event_objects_pending]
   bool(CLAP_ABI *apply_event_object)(const clap_plugin_t *plugin,
                                      bool is_redo,
                                      const clap_istream_t *apply_object_stream,
                                      const clap_ostream_t *exchange_object_stream);
} clap_plugin_undo_redo_t;

typedef struct clap_host_undo_redo {
   // After the plugin state has changed internally, the plugin must call this function to
   // tell the host that an event object is pending to be pulled from the plugin.
   // The host then must first check has_pending_event_object and call pull_next_pending_event_object
   // until has_pending_event_object returns false.
   // The plugin does not need to re-send mark_event_objects_pending until the host has
   // pulled all pending event objects from the plugin.
   // [main-thread]
   void(CLAP_ABI *mark_event_objects_pending)(const clap_host_t *host);
} clap_host_undo_redo_t;

#ifdef __cplusplus
}
#endif
