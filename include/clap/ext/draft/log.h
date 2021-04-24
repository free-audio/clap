#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_LOG "clap/draft/log"

enum clap_log_severity {
   CLAP_LOG_DEBUG = 0,
   CLAP_LOG_INFO = 1,
   CLAP_LOG_WARNING = 2,
   CLAP_LOG_ERROR = 3,
   CLAP_LOG_FATAL = 4,

   // This severity should be used to report misbehaviour of the host
   CLAP_LOG_HOST_MISBEHAVING = 5,
};

struct clap_host_log {
   // Log a message through the host.
   // [thread-safe]
   void (*log)(struct clap_host *     host,
               struct clap_plugin *   plugin,
               enum clap_log_severity severity,
               const char *           msg);
};

#ifdef __cplusplus
}
#endif