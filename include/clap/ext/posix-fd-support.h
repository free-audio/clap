#pragma once

#include "../plugin.h"

static CLAP_CONSTEXPR const char CLAP_EXT_POSIX_FD_SUPPORT[] = "clap.posix-fd-support";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // IO events flags
   CLAP_POSIX_FD_READ = 1 << 0,
   CLAP_POSIX_FD_WRITE = 1 << 1,
   CLAP_POSIX_FD_ERROR = 1 << 2,
};

typedef struct clap_plugin_posix_fd_support {
   // This callback is "level-triggered".
   // It means that a writable fd will continuously produce "on_fd()" events;
   // don't forget using modify_fd() to remove the write notification once you're
   // done writting.
   //
   // [main-thread]
   void (*on_fd)(const clap_plugin_t *plugin, int fd, int flags);
} clap_plugin_fd_support_t;

typedef struct clap_host_posix_fd_support {
   // [main-thread]
   bool (*register_fd)(const clap_host_t *host, int fd, int flags);

   // [main-thread]
   bool (*modify_fd)(const clap_host_t *host, int fd, int flags);

   // [main-thread]
   bool (*unregister_fd)(const clap_host_t *host, int fd);
} clap_host_posix_fd_support_t;

#ifdef __cplusplus
}
#endif
