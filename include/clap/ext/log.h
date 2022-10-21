#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_LOG[] = "clap.log";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_LOG_DEBUG = 0,
   CLAP_LOG_INFO = 1,
   CLAP_LOG_WARNING = 2,
   CLAP_LOG_ERROR = 3,
   CLAP_LOG_FATAL = 4,

   // These severities should be used to report misbehaviour.
   // The plugin one can be used by a layer between the plugin and the host.
   CLAP_LOG_HOST_MISBEHAVING = 5,
   CLAP_LOG_PLUGIN_MISBEHAVING = 6,
};
typedef int32_t clap_log_severity;

typedef struct clap_host_log {
   // Log a message through the host.
   // [thread-safe]
   void(CLAP_ABI *log)(const clap_host_t *host, clap_log_severity severity, const char *msg);
} clap_host_log_t;

#ifdef __cplusplus
}
#endif
