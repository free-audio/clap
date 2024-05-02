#pragma once

#include "../../plugin.h"
#include "../../events.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_TRIGGERS[] = "clap.triggers/1";

#ifdef __cplusplus
extern "C" {
#endif

/// @page Trigger events
///
/// This extension enables the plugin to expose a set of triggers to the host.
///
/// Some examples for triggers:
/// - trigger an envelope which is independent of the notes
/// - trigger a sample-and-hold unit (maybe even per-voice)

enum {
   // Does this trigger support per note automations?
   CLAP_TRIGGER_IS_AUTOMATABLE_PER_NOTE_ID = 1 << 0,

   // Does this trigger support per key automations?
   CLAP_TRIGGER_IS_AUTOMATABLE_PER_KEY = 1 << 1,

   // Does this trigger support per channel automations?
   CLAP_TRIGGER_IS_AUTOMATABLE_PER_CHANNEL = 1 << 2,

   // Does this trigger support per port automations?
   CLAP_TRIGGER_IS_AUTOMATABLE_PER_PORT = 1 << 3,
};
typedef uint32_t clap_trigger_info_flags;

// Given that this extension is still draft, it'll use the event-registry and its own event
// namespace until we stabilize it.
//
// #include <clap/ext/event-registry.h>
//
// uint16_t CLAP_EXT_TRIGGER_EVENT_SPACE_ID = UINT16_MAX;
// if (host_event_registry->query(host, CLAP_EXT_TRIGGERS, &CLAP_EXT_TRIGGER_EVENT_SPACE_ID)) {
//   /* we can use trigger events */
// }
//
// /* later on */
// clap_event_trigger ev;
// ev.header.space_id = CLAP_EXT_TRIGGER_EVENT_SPACE_ID;
// ev.header.type = CLAP_EVENT_TRIGGER;

enum { CLAP_EVENT_TRIGGER = 0 };

typedef struct clap_event_trigger {
   clap_event_header_t header;

   // target trigger
   clap_id trigger_id; // @ref clap_trigger_info.id
   void   *cookie;     // @ref clap_trigger_info.cookie

   // target a specific note_id, port, key and channel, -1 for global
   int32_t note_id;
   int16_t port_index;
   int16_t channel;
   int16_t key;
} clap_event_trigger_t;

/* This describes a trigger */
typedef struct clap_trigger_info {
   // stable trigger identifier, it must never change.
   clap_id id;

   clap_trigger_info_flags flags;

   // in analogy to clap_param_info.cookie
   void *cookie;

   // displayable name
   char name[CLAP_NAME_SIZE];

   // the module path containing the trigger, eg:"sequencers/seq1"
   // '/' will be used as a separator to show a tree like structure.
   char module[CLAP_PATH_SIZE];
} clap_trigger_info_t;

typedef struct clap_plugin_triggers {
   // Returns the number of triggers.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Copies the trigger's info to trigger_info and returns true on success.
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_plugin_t *plugin,
                            uint32_t             index,
                            clap_trigger_info_t *trigger_info);
} clap_plugin_triggers_t;

enum {
   // The trigger info did change, use this flag for:
   // - name change
   // - module change
   // New info takes effect immediately.
   CLAP_TRIGGER_RESCAN_INFO = 1 << 0,

   // Invalidates everything the host knows about triggers.
   // It can only be used while the plugin is deactivated.
   // If the plugin is activated use clap_host->restart() and delay any change until the host calls
   // clap_plugin->deactivate().
   //
   // You must use this flag if:
   // - some triggers were added or removed.
   // - some triggers had critical changes:
   //   - is_per_note (flag)
   //   - is_per_key (flag)
   //   - is_per_channel (flag)
   //   - is_per_port (flag)
   //   - cookie
   CLAP_TRIGGER_RESCAN_ALL = 1 << 1,
};
typedef uint32_t clap_trigger_rescan_flags;

enum {
   // Clears all possible references to a trigger
   CLAP_TRIGGER_CLEAR_ALL = 1 << 0,

   // Clears all automations to a trigger
   CLAP_TRIGGER_CLEAR_AUTOMATIONS = 1 << 1,
};
typedef uint32_t clap_trigger_clear_flags;

typedef struct clap_host_triggers {
   // Rescan the full list of triggers according to the flags.
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host, clap_trigger_rescan_flags flags);

   // Clears references to a trigger.
   // [main-thread]
   void(CLAP_ABI *clear)(const clap_host_t       *host,
                         clap_id                  trigger_id,
                         clap_trigger_clear_flags flags);
} clap_host_triggers_t;

#ifdef __cplusplus
}
#endif
