#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"
#include "../../stream.h"

/// @page Undo/Redo
///
/// For each action that causes a change of parameter values and non-parameter state
/// (the 'plugin state'), the plugin provides an incremental change event object to the host.
/// These incremental change event objects then can be applied in reverse order in order to
/// undo those actions. If undone, the same objects can be used to redo the action again.
///
/// As opposed to the state extension, incremental change event objects must not contain the
/// whole plugin state but rather only the necessary data for undoing/redoing a single action
/// on top of the current plugin state.
///
/// Important: Pay attention to the 'Notes on using streams' as found in stream.h !

static const char CLAP_EXT_UNDO_REDO[] = "clap.undo-redo.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

// Information about a single incremental change event.
// The host can use context and action to embed a message into its own undo history.
typedef struct clap_incremental_change_event_description {
   // A brief, human-readable description of the event context, for example:
   //   - "Filter 1 > Cutoff"
   //   - "Param Seq A"
   // It should not contain the name of the plugin.
   char context[CLAP_NAME_SIZE];
   // A brief, human-readable description of what happened, for example:
   //   - "Set to 100%"
   //   - "Steps edited"
   char action[CLAP_NAME_SIZE];
} clap_incremental_change_event_description_t;

typedef struct clap_plugin_undo_redo{
   // Pulls an incremental change event object into stream.
   // The plugin must provide an event description together with the object.
   // If the host still holds a redo event object, it should compare the received
   // undo event object with the next redo object. If it is not equal, the host may
   // omit the next and all following redo event objects for that plugin.
   // before (besides it wants to support undo history branching).
   // Returns true if the incremental change event object was correctly saved.
   // [main-thread]
   bool(CLAP_ABI *pull_undo_event_object)(const clap_plugin_t *plugin,
                                          const clap_ostream_t *stream,
                                          clap_incremental_change_event_description_t* description);

   // Applies an incremental change event object from stream in order to undo the corresponding
   // plugin action.
   // The host should move the incremental change event object onto its redo stack.
   // Returns true if the incremental change event object was correctly applied.
   // The plugin must not call notify_undo as this is implied by returning true already.
   // [main-thread]
   bool(CLAP_ABI *perform_undo)(const clap_plugin_t *plugin,
                                const clap_istream_t *stream);

   // Applies an incremental change event object from stream in order to redo the corresponding
   // plugin action.
   // The host should move the incremental change event object onto its undo stack.
   // Returns true if the incremental change event object was correctly applied.
   // The plugin must not call mark_dirty_for_undo as this is implied by returning true already.
   // [main-thread]
   bool(CLAP_ABI *perform_redo)(const clap_plugin_t *plugin,
                                const clap_istream_t *stream);
} clap_plugin_undo_redo_t;

typedef struct clap_host_undo_redo {
   // Tell the host that the plugin state has changed internally and
   // an incremental change event object is ready to be pulled from the plugin.
   // The host must then call pull_undo_event_object.
   // [main-thread]
   void(CLAP_ABI *mark_dirty_for_undo)(const clap_host_t *host);

   // Tell the host that the plugin has performed a single undo step internally.
   // The host then must roll back the last undo event for this plugin instance.
   // If multiple undo steps have been performed at once inside the plugin,
   // it must call this function multiple times as well.
   // [main-thread]
   void(CLAP_ABI *notify_undo)(const clap_host_t *host);
} clap_host_undo_redo_t;

#ifdef __cplusplus
}
#endif
