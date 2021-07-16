#pragma once

#include "../clap.h"

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

   // This severity should be used to report misbehaviour of the host
   CLAP_LOG_HOST_MISBEHAVING = 5,
};
typedef int32_t clap_log_severity;

typedef struct clap_host_log {
   // Log a message through the host.
   // [thread-safe]
   void (*log)(const clap_host *host, clap_log_severity severity, const char *msg);
} clap_host_log;

#ifdef __cplusplus
}
#endif