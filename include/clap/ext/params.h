#pragma once

#include "../clap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_PARAMS "clap/params"

enum {
   CLAP_PARAM_FLOAT = 0, // uses value.d
   CLAP_PARAM_BOOL = 1,  // uses value.b
   CLAP_PARAM_INT = 2,   // uses value.i
   CLAP_PARAM_ENUM = 3,  // uses value.i
};
typedef int32_t clap_param_type;

/* This describes the parameter and provides the current value */
typedef struct clap_param_info {
   /* param info */
   clap_id id;
   char    name[CLAP_NAME_SIZE];     // the display name
   char    module[CLAP_MODULE_SIZE]; // the module containing the param, eg:
                                     // "/filters/moog"; '/' will be used as a
                                     // separator to show a tree like structure.

   bool is_per_note;    // does this param supports per note automations?
   bool is_per_channel; // does this param supports per channel automations?
   bool is_modulable;   // does this param
   bool is_used;        // is this parameter used by the patch?
   bool is_periodic;    // after the last value, go back to the first one
   bool is_locked;      // if true, the parameter can't be changed by the host
   bool is_automatable; // can the host send param event to change it in the process call?
   bool is_hidden;      // it implies is_automatable == true. Don't show it to the user.
   bool is_bypass;      // used to merge the plugin and host bypass button.

   /* value */
   clap_param_type  type;
   clap_param_value min_value;        // minimum plain value
   clap_param_value max_value;        // maximum plain value
   clap_param_value default_value;    // default plain value
   int32_t          enum_entry_count; // the number of values in the enum, if type is an enum
} clap_param_info;

typedef struct clap_plugin_params {
   // Returns the number of parameters.
   // [main-thread]
   int32_t (*count)(clap_plugin *plugin);

   // Copies the parameter's info to param_info and returns true on success.
   // [main-thread]
   bool (*get_info)(clap_plugin *plugin, int32_t param_index, clap_param_info *param_info);

   bool (*get_enum_value)(clap_plugin *     plugin,
                          clap_id           param_id,
                          int32_t           value_index,
                          clap_param_value *plain_value);

   // Gets the parameter plain value.
   // [main-thread]
   bool (*get_value)(clap_plugin *plugin, clap_id param_id, clap_param_value *plain_value);

   // Sets the parameter plain value.
   // If the plupin is activated, then the host must send a param event
   // in the next process call to update the audio processor.
   // [main-thread]
   bool (*set_value)(clap_plugin *    plugin,
                     clap_id          param_id,
                     clap_param_value plain_value,
                     clap_param_value plain_modulated_value);

   // Formats the display text for the given parameter value.
   // [thread-safe,lock-wait-free]
   bool (*value_to_text)(clap_plugin *    plugin,
                         clap_id          param_id,
                         clap_param_value plain_value,
                         char *           display,
                         uint32_t         size);

   bool (*text_to_value)(clap_plugin *     plugin,
                         clap_id           param_id,
                         const char *      display,
                         clap_param_value *plain_value);
} clap_plugin_params;

typedef struct clap_host_params {
   /* [main-thread] */
   void (*touch_begin)(clap_host *host, clap_id param_id);

   /* [main-thread] */
   void (*touch_end)(clap_host *host, clap_id param_id);

   // If the plugin is activated, the host must send a parameter update
   // in the next process call to update the audio processor.
   // Only for value changes that happens in the gui.
   // [main-thread]
   void (*changed)(clap_host *host, clap_id param_id, clap_param_value plain_value);

   // [main-thread]
   void (*rescan)(clap_host *host, bool did_parameter_list_change);
   void (*rescan_params)(clap_host *host, const uint32_t *indexes, uint32_t count);
} clap_host_params;

#ifdef __cplusplus
}
#endif