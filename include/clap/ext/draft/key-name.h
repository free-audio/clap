#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_NOTE_NAME "clap/draft/note-name"

struct clap_note_name {
   char   name[CLAP_NAME_SIZE];
   int8_t key;
   int8_t channel; // -1 for every channels
};

struct clap_plugin_note_name {
   // Return the number of note names
   // [main-thread]
   int (*count)(struct clap_plugin *plugin);

   // Returns true on success and stores the result into note_name
   // [main-thread]
   bool (*get)(struct clap_plugin *   plugin,
               int                    index,
               struct clap_note_name *note_name);
};

struct clap_host_note_name {
   // Informs the host that the note names has changed.
   // [main-thread]
   void (*changed)(struct clap_host *host, struct clap_plugin *plugin);
};

#ifdef __cplusplus
}
#endif