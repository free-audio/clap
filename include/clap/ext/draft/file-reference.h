#pragma once

#include "../../clap.h"

#define CLAP_EXT_FILE_REFERENCE "clap/file-reference"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_file_reference {
   clap_id resource_id;
   char    path[CLAP_PATH_SIZE];
} clap_file_reference;

typedef struct clap_plugin_file_reference {
   // returns the number of file reference this plugin has
   // [main-thread]
   uint32_t (*count)(clap_plugin *plugin);

   // gets the file reference at index
   // returns true on success
   // [main-thread]
   bool (*get)(clap_plugin *plugin, uint32_t index, clap_file_reference *file_reference);

   // updates the path to a file reference
   // [main-thread]
   bool (*set)(clap_plugin *plugin, clap_id resource_id, const char *path);
} clap_plugin_file_reference;

typedef struct clap_host_file_reference {
   // informs the host that the file references have changed, the host should schedule a full rescan
   // [main-thread]
   void (*changed)(clap_host *host);
} clap_host_file_reference;

#ifdef __cplusplus
}
#endif
