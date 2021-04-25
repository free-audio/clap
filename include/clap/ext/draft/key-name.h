#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_NOTE_NAME "clap/draft/note-name"

typedef struct clap_note_name {
   char   name[CLAP_NAME_SIZE];
   int8_t key;
   int8_t channel; // -1 for every channels
} clap_note_name;

typedef struct clap_plugin_note_name {
   // Return the number of note names
   // [main-thread]
   int (*count)(clap_plugin *plugin);

   // Returns true on success and stores the result into note_name
   // [main-thread]
   bool (*get)(clap_plugin *plugin, int index, clap_note_name *note_name);
} clap_plugin_note_name;

typedef struct clap_host_note_name {
   // Informs the host that the note names has changed.
   // [main-thread]
   void (*changed)(clap_host *host, clap_plugin *plugin);
} clap_host_note_name;

#ifdef __cplusplus
}
#endif