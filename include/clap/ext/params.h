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
   clap_param_type  type;             // this field is not allowed to change for a given param id
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

enum {
   // The parameter values did change. Typically what happens when loading a preset.
   // The host will scan the parameter value and value_to_text.
   // The host will not record those changes as automation points.
   // New values takes effect immediately.
   CLAP_PARAM_RESCAN_VALUES = 1 << 0,

   // The parameter info did change, use this flag for:
   // - name change
   // - module change
   // - is_modulable
   // - is_used
   // - is_periodic
   // - is_hidden
   // New info takes effect immediately.
   CLAP_PARAM_RESCAN_INFO = 1 << 1,

   // This invalidates everything the host knows about parameters and takes effect after the host
   // did deactivate the plugin if it was activated.
   //
   // You must use this flag if:
   // - some parameters were added or removed.
   // - some parameters had critical changes which requieres to invalidate the host queues:
   //   - is_per_note
   //   - is_per_channel
   //   - is_automatable
   //   - is_locked
   //   - is_bypass
   //   - min_value
   //   - max_value
   //   - enum definition (enum_entry_count or enum value)
   //
   // The plugin can't perform the parameter list change immediately:
   // 1. host_params->rescan(host, CLAP_PARAM_RESCAN_ALL);
   // 2. the host needs to flush its parameters update queues and suspend
   //    the plugin execution in the audio engine
   // 3. the host calls plugin->set_active(plugin, 0, false); and the plugin
   //    can switch to its new parameters
   // 4. the host scans all the parameters
   // 5. the host activates the plugin
   CLAP_PARAM_RESCAN_ALL = 1 << 2,
};

typedef struct clap_host_params {
   /* [main-thread] */
   void (*adjust_begin)(clap_host *host, clap_id param_id);

   // If the plugin is activated, the host must send a parameter update
   // in the next process call to update the audio processor.
   // Only for value changes that happens in the gui.
   // [main-thread]
   void (*adjust)(clap_host *host, clap_id param_id, clap_param_value plain_value);

   /* [main-thread] */
   void (*adjust_end)(clap_host *host, clap_id param_id);

   // Rescan the full list of parameters according to the flags.
   // [main-thread]
   void (*rescan)(clap_host *host, uint32_t flags);

   // Only rescan the given subset of parameters according to the flags.
   // [main-thread]
   void (*rescan_params)(clap_host *host, uint32_t flags, const uint32_t *indexes, uint32_t count);
} clap_host_params;

#ifdef __cplusplus
}
#endif