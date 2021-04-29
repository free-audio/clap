#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_PROGRAM "clap/draft/program"

typedef struct clap_program_info {
   int32_t bank;
   int32_t number;
   char    name[CLAP_NAME_SIZE];
} clap_program_info;

//
typedef struct clap_plugin_program {
   int32_t (*count)(clap_plugin *plugin);
   bool (*get_info)(clap_plugin *plugin, int32_t index, clap_program_info *info);
   bool (*clear_program)(clap_plugin *plugin, int32_t bank, int32_t program);

   // Stores the current state into a program
   bool (*store_program)(clap_plugin *plugin, int32_t bank, int32_t program, const char *name);

   bool (*rename_program)(clap_plugin *plugin, int32_t bank, int32_t program, const char *name);

   bool (*copy_program)(clap_plugin *plugin,
                        int32_t      src_bank,
                        int32_t      src_program,
                        int32_t      dst_bank,
                        int32_t      dst_program);

   bool (*move_program)(clap_plugin *plugin,
                        int32_t      src_bank,
                        int32_t      src_program,
                        int32_t      dst_bank,
                        int32_t      dst_program);
} clap_plugin_program;

#ifdef __cplusplus
}
#endif