#pragma once

#include <stddef.h>

#include "../clap.h"

static CLAP_CONSTEXPR const char CLAP_EXT_FD_SUPPORT[] = "clap.fd-support";

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
typedef void *clap_fd;
#else
typedef int clap_fd;
#endif

enum {
   // IO events
   CLAP_FD_READ = 1 << 0,
   CLAP_FD_WRITE = 1 << 1,
   CLAP_FD_ERROR = 1 << 2,
};
typedef uint32_t clap_fd_flags;

typedef struct clap_plugin_fd_support {
   // This callback is "level-triggered".
   // It means that a writable fd will continuously produce "on_fd()" events;
   // don't forget using modify_fd() to remove the write notification once you're
   // done writting.
   //
   // [main-thread]
   void (*on_fd)(const clap_plugin *plugin, clap_fd fd, clap_fd_flags flags);
} clap_plugin_fd_support;

typedef struct clap_host_fd_support {
   // [main-thread]
   bool (*register_fd)(const clap_host *host, clap_fd fd, clap_fd_flags flags);

   // [main-thread]
   bool (*modify_fd)(const clap_host *host, clap_fd fd, clap_fd_flags flags);

   // [main-thread]
   bool (*unregister_fd)(const clap_host *host, clap_fd fd);
} clap_host_fd_support;

#ifdef __cplusplus
}
#endif