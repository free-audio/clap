#ifndef CLAP_EXT_PARAMS_H
# define CLAP_EXT_PARAMS_H

# include "../clap.h"

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
  union clap_param_value  value;
  union clap_param_value  min;
  union clap_param_value  max;
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

#endif /* !CLAP_EXT_PARAMS_H */
