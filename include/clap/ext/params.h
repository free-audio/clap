#pragma once

#include "../clap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLAP_EXT_PARAMS "clap/params"
#define CLAP_ROOT_MODULE_ID ""

enum clap_param_type {
   CLAP_PARAM_FLOAT = 0, // uses value.d
   CLAP_PARAM_BOOL = 1,  // uses value.b
   CLAP_PARAM_INT = 2,   // uses value.i
   CLAP_PARAM_ENUM = 3,  // uses value.i
};

/* This describes the parameter and provides the current value */
struct clap_param {
   /* param info */
   char id[CLAP_ID_SIZE]; // a string which identify the param
   char module[CLAP_ID_SIZE];
   char name[CLAP_NAME_SIZE]; // the display name
   char desc[CLAP_DESC_SIZE];
   char display[CLAP_DISPLAY_SIZE]; // the text used to display the value
   bool is_per_note;
   bool is_per_channel;
   bool is_used;     // is this parameter used by the patch?
   bool is_periodic; // after the last value, go back to the first one
   bool is_locked;   // if true, the parameter can't be changed by the host
   bool is_automatable;

   /* value */
   enum clap_param_type   type;
   union clap_param_value plain_value; // current value
   union clap_param_value normalized_value;
   union clap_param_value min;   // minimum value
   union clap_param_value max;   // maximum value
   union clap_param_value deflt; // default value
};

struct clap_param_module {
   char id[CLAP_ID_SIZE];
   char parent_id[CLAP_ID_SIZE];
   char name[CLAP_NAME_SIZE];
   char desc[CLAP_DESC_SIZE];
};

struct clap_plugin_params {
   /* Returns the number of parameters.
    * [main-thread] */
   int32_t (*count)(struct clap_plugin *plugin);

   /* Copies the parameter's info to param and returns true.
    * If index is greater or equal to the number then return false.
    * [main-thread] */
   bool (*get_param)(struct clap_plugin *plugin,
                     int32_t             index,
                     struct clap_param * param);

   /* Copies the module's info to module and returns true on success.
    * [main-thread] */
   bool (*get_module)(struct clap_plugin *      plugin,
                      const char *              module_id,
                      struct clap_param_module *module);

   // [thread-safe,lock-free]
   double (*plain_to_normalized)(struct clap_plugin *plugin,
                                 int32_t             index,
                                 double              plain_value);
   double (*normalized_to_plain)(struct clap_plugin *plugin,
                                 int32_t             index,
                                 double              normalized_value);

   // Sets the parameter value.
   // If the plupin is activated, then the host must send a param event
   // in the next process call to update the audio processor.
   // [main-thread]
   void (*set_parameter_value)(struct clap_plugin *   plugin,
                               int32_t                index,
                               union clap_param_value plain_value);
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

   /* [main-thread] */
   void (*rescan)(struct clap_host *host, struct clap_plugin *plugin);
};

#ifdef __cplusplus
}
#endif