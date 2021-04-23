#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../clap.h"

#define CLAP_EXT_POLL "clap/draft/ev-loop"

enum {
   CLAP_EV_LOOP_READ = 1,
   CLAP_EV_LOOP_WRITE = 2,
};

typedef void (*clap_fd_callback)(struct clap_plugin *plugin, int fd, int flags);

typedef void (*clap_timer_callback)(struct clap_plugin *plugin,
                                    uint64_t            timer_id);

struct clap_host_ev_loop {
   // [main-thread]
   bool (*register_timer)(struct clap_host *  host,
                          struct clap_plugin *plugin,
                          int64_t             period_ms,
                          clap_timer_callback callback,
                          uint64_t *          timer_id);

   // [main-thread]
   bool (*unregister_timer)(struct clap_host *  host,
                            struct clap_plugin *plugin,
                            uint64_t *          timer_id);

   // [main-thread]
   bool (*register_fd)(struct clap_host *  host,
                       struct clap_plugin *plugin,
                       int                 fd,
                       int                 flags,
                       clap_fd_callback    callback);

   // [main-thread]
   bool (*unregister_fd)(struct clap_host *  host,
                         struct clap_plugin *plugin,
                         int                 fd);
};

#ifdef __cplusplus
}
#endif