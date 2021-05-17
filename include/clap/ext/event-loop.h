#pragma once

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "../clap.h"

#define CLAP_EXT_EVENT_LOOP "clap/event-loop"

#ifdef _WIN32
typedef void *clap_fd;
#else
typedef int clap_fd;
#endif

enum {
   CLAP_FD_READ = 1 << 0,
   CLAP_FD_WRITE = 1 << 1,
   CLAP_FD_ERROR = 1 << 2,
};

typedef struct clap_plugin_event_loop {
   // [main-thread]
   void (*on_timer)(clap_plugin *plugin, clap_id timer_id);

   // [main-thread]
   void (*on_fd)(clap_plugin *plugin, clap_fd fd, uint32_t flags);
} clap_plugin_event_loop;

typedef struct clap_host_event_loop {
   // [main-thread]
   bool (*register_timer)(clap_host *host, uint32_t period_ms, clap_id *timer_id);

   // [main-thread]
   bool (*unregister_timer)(clap_host *host, clap_id timer_id);

   // [main-thread]
   bool (*register_fd)(clap_host *host, clap_fd fd, uint32_t flags);

   // [main-thread]
   bool (*modify_fd)(clap_host *host, clap_fd fd, uint32_t flags);

   // [main-thread]
   bool (*unregister_fd)(clap_host *host, clap_fd fd);
} clap_host_event_loop;

#ifdef __cplusplus
}
#endif