#pragma once

#include <stddef.h>

#include "../clap.h"

static CLAP_CONSTEXPR const char CLAP_EXT_EVENT_LOOP[] = "clap/event-loop";

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

typedef struct clap_plugin_event_loop {
   // [main-thread]
   void (*on_timer)(const clap_plugin *plugin, clap_id timer_id);

   // This callback is "level-triggered".
   // It means that a writable fd will continuously produce "on_fd()" events;
   // don't forget using modify_fd() to remove the write notification once you're
   // done writting.
   //
   // [main-thread]
   void (*on_fd)(const clap_plugin *plugin, clap_fd fd, clap_fd_flags flags);
} clap_plugin_event_loop;

typedef struct clap_host_event_loop {
   // Registers a periodic timer.
   // The host may adjust the period if it is under a certain threshold.
   // 30 Hz should be allowed.
   // [main-thread]
   bool (*register_timer)(const clap_host *host, uint32_t period_ms, clap_id *timer_id);

   // [main-thread]
   bool (*unregister_timer)(const clap_host *host, clap_id timer_id);

   // [main-thread]
   bool (*register_fd)(const clap_host *host, clap_fd fd, clap_fd_flags flags);

   // [main-thread]
   bool (*modify_fd)(const clap_host *host, clap_fd fd, clap_fd_flags flags);

   // [main-thread]
   bool (*unregister_fd)(const clap_host *host, clap_fd fd);
} clap_host_event_loop;

#ifdef __cplusplus
}
#endif