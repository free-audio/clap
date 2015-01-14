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
  CLAP_PARAM_GROUP = 0, // no value for this one
  CLAP_PARAM_BOOL  = 1, // uses value.b
  CLAP_PARAM_FLOAT = 2, // uses value.f
  CLAP_PARAM_INT   = 3, // uses value.i
  CLAP_PARAM_ENUM  = 4, // uses value.i
};

enum clap_param_scale
{
  CLAP_PARAM_LINEAR = 0,
  CLAP_PARAM_LOG    = 1,
};

struct clap_param
{
  /* tree fields */
  uint32_t index;  // parameter's index
  uint32_t parent; // parent's index, -1 for no parent

  /* param info */
  char                    id[CLAP_ID_SIZE]; // a string which identify the param
  char                    name[CLAP_NAME_SIZE]; // the display name
  char                    desc[CLAP_DESC_SIZE];
  char                    display[CLAP_DISPLAY_SIZE]; // the text used to display the value
  enum clap_param_type    type;
  bool                    is_per_note;
  bool                    is_used; // is this parameter used by the patch?
  bool                    is_periodic;
  bool                    is_locked; // if true, the parameter can't be changed by
                                     // the host
  union clap_param_value  value;
  union clap_param_value  min;
  union clap_param_value  max;
  union clap_param_value  deflt; // default value
  enum clap_param_scale   scale;
};

struct clap_plugin_params
{
  /* Returns the number of parameters. */
  uint32_t (*count)(struct clap_plugin *plugin);

  /* Copies the parameter's info to param and returns true.
   * If index is greater or equal to the number then return false. */
  bool (*get)(struct clap_plugin *plugin,
              uint32_t            index,
              struct clap_param  *param);
};

/* Helper that will serialize the plugin's parameters value into the buffer.
 * (*size) must be set to the size of the buffer.
 * At return, (*size) contains the number of bytes used and returns true on
 * success, the only possible error is that the buffer is too small.
 */
static inline bool
clap_plugin_params_save(struct clap_plugin *plugin,
                        uint8_t            *buffer,
                        uint32_t           *size);

/* Helper that will deserialize parameters value from the buffer
 * and send CLAP_EVENT_PARAM_SET to the plugin to restore them.
 * The steady_time is required because the plugin can discard
 * events from the past.
 */
static inline void
clap_plugin_params_restore(struct clap_plugin *plugin,
                           const uint8_t      *buffer,
                           uint32_t            size,
			   uint64_t            steady_time);

# include "params.c"

#ifdef __cplusplus
}
#endif

#endif /* !CLAP_EXT_PARAMS_H */
