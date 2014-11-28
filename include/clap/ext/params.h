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
  enum clap_param_type    type;
  char                    id[32];   // a string which identify the param
  char                    name[CLAP_NAME_SIZE]; // the display name
  char                    desc[CLAP_DESC_SIZE];
  bool                    is_per_note;
  char                    display_text[CLAP_DISPLAY_SIZE]; // the text used to display the value
  bool                    is_used; // is this parameter used by the patch?
  union clap_param_value  value;
  union clap_param_value  min;
  union clap_param_value  max;
  enum clap_param_scale   scale;
};

struct clap_plugin_params
{
  /* Returns a newly allocated parameters tree. The caller has to free it. */
  uint32_t (*get_params_count)(struct clap_plugin *plugin);
  bool (*get_param)(struct clap_plugin *plugin,
                    uint32_t            index,
                    struct clap_param  *param);
};

#endif /* !CLAP_EXT_PARAMS_H */
