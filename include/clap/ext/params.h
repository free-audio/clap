#ifndef CLAP_EXT_PARAMS_H
# define CLAP_EXT_PARAMS_H

# include "../clap.h"
# include "../serialize/serialize.h"

#ifdef __cplusplus
extern "C" {
#endif

# define CLAP_EXT_PARAMS "clap/params"

enum clap_param_type
{
  CLAP_PARAM_FLOAT = 0, // uses value.f
  CLAP_PARAM_BOOL  = 1, // uses value.b
  CLAP_PARAM_INT   = 2, // uses value.i
  CLAP_PARAM_ENUM  = 3, // uses value.i
};

enum clap_param_scale
{
  CLAP_PARAM_SCALE_LINEAR = 0,
  CLAP_PARAM_SCALE_LOG    = 1,
  CLAP_PARAM_SCALE_EXP    = 2,
};

struct clap_param
{
  int32_t index;  // parameter's index

  /* param info */
  char                    module_id[CLAP_ID_SIZE];
  char                    id[CLAP_ID_SIZE]; // a string which identify the param
  char                    name[CLAP_NAME_SIZE]; // the display name
  char                    desc[CLAP_DESC_SIZE];
  char                    display[CLAP_DISPLAY_SIZE]; // the text used to display the value
  enum clap_param_type    type;
  bool                    is_per_note;
  bool                    is_per_channel;
  bool                    is_used;   // is this parameter used by the patch?
  bool                    is_periodic;
  bool                    is_locked; // if true, the parameter can't be changed by
                                     // the host
  union clap_param_value  value;
  union clap_param_value  min;
  union clap_param_value  max;
  union clap_param_value  deflt; // default value
  enum  clap_param_scale  scale;
};

struct clap_param_module
{
  char id[CLAP_ID_SIZE];
  char name[CLAP_NAME_SIZE];
  char desc[CLAP_DESC_SIZE];
};

struct clap_plugin_params
{
  /* Returns the number of parameters. */
  int32_t (*count)(struct clap_plugin *plugin);

  /* Copies the parameter's info to param and returns true.
   * If index is greater or equal to the number then return false. */
  bool (*get_param)(struct clap_plugin *plugin,
                    int32_t             index,
                    struct clap_param  *param);

  bool (*get_module)(struct clap_plugin       *plugin,
                     const char               *module_id,
                     struct clap_param_module *module);
};

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

#ifdef __cplusplus
}
#endif

#endif /* !CLAP_EXT_PARAMS_H */
