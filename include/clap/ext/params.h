#pragma once

#include "../clap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_PARAMS "clap/params"

typedef enum clap_param_type {
   CLAP_PARAM_FLOAT = 0, // uses value.d
   CLAP_PARAM_BOOL = 1,  // uses value.b
   CLAP_PARAM_INT = 2,   // uses value.i
   CLAP_PARAM_ENUM = 3,  // uses value.i
} clap_param_type;

/* This describes the parameter and provides the current value */
typedef struct clap_param_info {
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
   bool    is_hidden;
   bool    is_bypass;

   /* value */
   clap_param_type  type;
   clap_param_value min_value;     // minimum plain value
   clap_param_value max_value;     // maximum plain value
   clap_param_value default_value; // default plain value
} clap_param_info;

typedef struct clap_plugin_params {
   // Returns the number of parameters.
   // [main-thread]
   int32_t (*count)(clap_plugin *plugin);

   // Copies the parameter's info to param_info and returns true on success.
   // [main-thread]
   bool (*get_info)(clap_plugin *    plugin,
                    int32_t          param_index,
                    clap_param_info *param_info);

   // Gets the parameter plain value.
   // [main-thread]
   clap_param_value (*get_value)(clap_plugin *plugin, int32_t param_index);

   // Sets the parameter plain value.
   // If the plupin is activated, then the host must send a param event
   // in the next process call to update the audio processor.
   // [main-thread]
   void (*set_value)(clap_plugin *    plugin,
                     int32_t          param_index,
                     clap_param_value plain_value);

   // Normalization only exists for float values
   // [thread-safe,lock-wait-free]
   double (*plain_to_norm)(clap_plugin *plugin,
                           int32_t      param_index,
                           double       plain_value);
   double (*norm_to_plain)(clap_plugin *plugin,
                           int32_t      param_index,
                           double       normalized_value);

   // Formats the display text for the given parameter value.
   // [thread-safe,lock-wait-free]
   bool (*value_to_text)(clap_plugin *    plugin,
                         int32_t          param_index,
                         clap_param_value plain_value,
                         char *           display,
                         uint32_t         size);

   bool (*text_to_value)(clap_plugin *     plugin,
                         int32_t           param_index,
                         const char *      display,
                         clap_param_value *plain_value);
} clap_plugin_params;

typedef struct clap_host_params {
   /* [main-thread] */
   void (*touch_begin)(clap_host *host, clap_plugin *plugin, int32_t index);

   /* [main-thread] */
   void (*touch_end)(clap_host *host, clap_plugin *plugin, int32_t index);

   // If the plugin is activated, the host must send a parameter update
   // in the next process call to update the audio processor.
   // Only for value changes that happens in the gui.
   // [main-thread]
   void (*changed)(clap_host *      host,
                   clap_plugin *    plugin,
                   int32_t          index,
                   clap_param_value plain_value);

   // [main-thread]
   void (*rescan)(clap_host *host, clap_plugin *plugin);
   void (*rescan_params)(clap_host *     host,
                         clap_plugin *   plugin,
                         const uint32_t *indexes,
                         uint32_t        count);
} clap_host_params;

#ifdef __cplusplus
}
#endif