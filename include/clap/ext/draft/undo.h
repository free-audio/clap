#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"

static CLAP_CONSTEXPR const char CLAP_EXT_UNDO[] = "clap.undo.draft/0";

// Describes an undo step
typedef struct clap_undo_info {
   // This is the unique identifier of this undo step.
   // It is valid until loading a state or destroying the plugin.
   clap_id id;

   // A short string which describes the corresponding change.
   char description[CLAP_NAME_SIZE];
} clap_undo_info_t;

typedef struct clap_plugin_undo {
   // returns true if an undo/redo step exists and the info were provided
   // [main-thread]
   bool (*get_current_undo_info)(clap_plugin_t *plugin, clap_undo_info_t *info);
   bool (*get_current_redo_info)(clap_plugin_t *plugin, clap_undo_info_t *info);

   // request the plugin to perform an undo operation (async)
   // returns true if the request is being processed, false otherwise.
   // when returning true, the plugin must call clap_host_undo->after_undo_request() once the
   // request is completed or failed.
   //
   // The plugin should only perform the operation if the current
   // undo/redo operation matches the given id; this is because of the asynchronous nature of the
   // task and to avoid race conditions if the plugin's undo manager lives in a different thread.
   //
   // [main-thread]
   bool (*request_undo)(clap_plugin_t *plugin, clap_id id);
   bool (*request_redo)(clap_plugin_t *plugin, clap_id id);
} clap_plugin_undo_t;

typedef struct clap_host_undo {
   // Marks the begining and end of a change which will lead to the creation of an undo step.
   // [main-thread]
   void (*begin_changes)(clap_host_t *host, const clap_undo_info_t *info);
   void (*end_changes)(clap_host_t *host, const clap_undo_info_t *info);

   // Marks the beginning and end of processing an undo change.
   // [main-thread]
   void (*begin_undo)(clap_host_t *host, const clap_undo_info_t *info);
   void (*end_undo)(clap_host_t *host, const clap_undo_info_t *info);

   // Marks the beginning and end of processing a redo change.
   // [main-thread]
   void (*begin_redo)(clap_host_t *host, const clap_undo_info_t *info);
   void (*end_redo)(clap_host_t *host, const clap_undo_info_t *info);

   // A destructive change happened which makes it impossible to perform an undo.
   // The entire plugin's undo/redo stack has been cleared.
   // [main-thread]
   void (*after_destructive_change)(clap_host_t *host);

   // Callbacks for clap_plugin_undo->request_*()
   // [main-thread]
   void (*after_undo_request)(clap_host_t *host, clap_id id, bool succeed, const char *error_msg);
   void (*after_redo_request)(clap_host_t *host, clap_id id, bool succeed, const char *error_msg);
} clap_host_undo_t;
