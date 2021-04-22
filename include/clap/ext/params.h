#pragma once

#include "../clap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_PARAMS "clap/params"

enum clap_param_type {
   CLAP_PARAM_FLOAT = 0, // uses value.d
   CLAP_PARAM_BOOL = 1,  // uses value.b
   CLAP_PARAM_INT = 2,   // uses value.i
   CLAP_PARAM_ENUM = 3,  // uses value.i
};

/* This describes the parameter and provides the current value */
struct clap_param_info {
   /* param info */
   int32_t index;
   int32_t id;                   // a string which identify the param
   char    name[CLAP_NAME_SIZE]; // the display name
   bool    is_per_note;
   bool    is_per_channel;
   bool    is_used;     // is this parameter used by the patch?
   bool    is_periodic; // after the last value, go back to the first one
   bool    is_locked;   // if true, the parameter can't be changed by the host
   bool    is_automatable;

   /* value */
   enum clap_param_type   type;
   union clap_param_value value;         // current plain value
   union clap_param_value min_value;     // minimum plain value
   union clap_param_value max_value;     // maximum plain value
   union clap_param_value default_value; // default plain value
};

struct clap_plugin_params {
   // Returns the number of parameters.
   // [main-thread]
   int32_t (*count)(struct clap_plugin *plugin);

   // Copies the parameter's info to param_info and returns true on success.
   // [main-thread]
   bool (*get_param_info)(struct clap_plugin *    plugin,
                          int32_t                 param_index,
                          struct clap_param_info *param_info);

   // Gets the parameter plain value.
   // [main-thread]
   union clap_param_value (*get_param_value)(struct clap_plugin *plugin,
                                             int32_t             param_index);

   // Sets the parameter plain value.
   // If the plupin is activated, then the host must send a param event
   // in the next process call to update the audio processor.
   // [main-thread]
   void (*set_param_value)(struct clap_plugin *   plugin,
                           int32_t                param_index,
                           union clap_param_value plain_value);

   // Normalization only exists for float values
   // [thread-safe,lock-free]
   double (*plain_to_norm)(struct clap_plugin *plugin,
                           int32_t             param_index,
                           double              plain_value);
   double (*norm_to_plain)(struct clap_plugin *plugin,
                           int32_t             param_index,
                           double              normalized_value);

   // Formats the display text for the given parameter value.
   // [thread-safe,lock-free]
   bool (*get_param_display)(struct clap_plugin *   plugin,
                             int32_t                param_index,
                             union clap_param_value plain_value,
                             char *                 display,
                             uint32_t               size);
};

struct clap_host_params {
   /* [main-thread] */
   void (*touch_begin)(struct clap_host *  host,
                       struct clap_plugin *plugin,
                       int32_t             index);

   /* [main-thread] */
   void (*touch_end)(struct clap_host *  host,
                     struct clap_plugin *plugin,
                     int32_t             index);

   // If the plugin is activated, the host must send a parameter update
   // in the next process call to update the audio processor.
   // Only for value changes that happens in the gui.
   // [main-thread]
   void (*changed)(struct clap_host *     host,
                   struct clap_plugin *   plugin,
                   int32_t                index,
                   union clap_param_value plain_value,
                   bool                   is_recordable);

   // [main-thread]
   void (*rescan)(struct clap_host *host, struct clap_plugin *plugin);
};

#ifdef __cplusplus
}
#endif