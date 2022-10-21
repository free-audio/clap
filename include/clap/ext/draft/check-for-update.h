#pragma once

#include "../../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_CHECK_FOR_UPDATE[] = "clap.check_for_update.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_check_for_update_info {
   const char *version;      // latest version
   const char *release_date; // YYYY-MM-DD
   const char *url;          // url to a download page which the user can visit

   bool is_preview; // true if this version is a preview release
} clap_check_for_update_info_t;

typedef struct clap_plugin_check_for_update {
   // [main-thread]
   void(CLAP_ABI *check)(const clap_plugin_t *plugin, bool include_preview);
} clap_plugin_check_for_update_t;

typedef struct clap_host_check_for_update {
   // [main-thread]
   void(CLAP_ABI *on_new_version)(const clap_host_t                  *host,
                                  const clap_check_for_update_info_t *update_info);
} clap_host_check_for_update_t;

#ifdef __cplusplus
}
#endif
