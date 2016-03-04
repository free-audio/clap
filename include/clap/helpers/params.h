#ifndef CLAP_HELPERS_PARAMS_H
# define CLAP_HELPERS_PARAMS_H

# include "../ext/params.h"
# include "serialize.h"

/* Helper that will serialize the plugin's parameters value into the buffer.
 * (*size) must be set to the size of the buffer.
 * At return, (*size) contains the number of bytes used and returns true on
 * success, the only possible error is that the buffer is too small.
 */
static inline bool
clap_plugin_params_save(struct clap_plugin *plugin,
                        uint8_t            *buffer,
                        int32_t            *size);

/* Helper that will deserialize parameters value from the buffer
 * and send CLAP_EVENT_PARAM_SET to the plugin to restore them.
 * The steady_time is required because the plugin can discard
 * events from the past.
 */
static inline void
clap_plugin_params_restore(struct clap_plugin *plugin,
                           const uint8_t      *buffer,
                           int32_t             size,
			   int64_t             steady_time);

# include "params.c"

#endif /* !CLAP_HELPERS_PARAMS_H */
