#pragma once

#include "../plugin.h"

// This extension let your plugin hook itself into the host select/poll/epoll/kqueue reactor.
// This is useful to handle asynchronous I/O on the main thread.
static CLAP_CONSTEXPR const char CLAP_EXT_POSIX_FD_SUPPORT[] = "clap.posix-fd-support";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // IO events flags, they can be used to form a mask which describes:
   // - which events you are interested in (register_fd/modify_fd)
   // - which events happened (on_fd)
   CLAP_POSIX_FD_READ = 1 << 0,
   CLAP_POSIX_FD_WRITE = 1 << 1,
   CLAP_POSIX_FD_ERROR = 1 << 2,
};
typedef uint32_t clap_posix_fd_flags_t;

typedef struct clap_plugin_posix_fd_support {
   // This callback is "level-triggered".
   // It means that a writable fd will continuously produce "on_fd()" events;
   // don't forget using modify_fd() to remove the write notification once you're
   // done writing.
   //
   // [main-thread]
   void(CLAP_ABI *on_fd)(const clap_plugin_t *plugin, int fd, clap_posix_fd_flags_t flags);
} clap_plugin_posix_fd_support_t;

typedef struct clap_host_posix_fd_support {
   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *register_fd)(const clap_host_t *host, int fd, clap_posix_fd_flags_t flags);

   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *modify_fd)(const clap_host_t *host, int fd, clap_posix_fd_flags_t flags);

   // Returns true on success.
   // [main-thread]
   bool(CLAP_ABI *unregister_fd)(const clap_host_t *host, int fd);
} clap_host_posix_fd_support_t;

#ifdef __cplusplus
}
#endif
