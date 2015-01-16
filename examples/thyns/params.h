#ifndef PARAMS_H
# define PARAMS_H

struct thyns_params
{
  union clap_param_value values[0];
  union clap_param_value osc1[THYNS_OSC_PARAM_COUNT];
  union clap_param_value osc2[THYNS_OSC_PARAM_COUNT];
  union clap_param_value filt[THYNS_FILT_PARAM_COUNT];
  union clap_param_value amp_env[THYNS_ENV_PARAM_COUNT];
  union clap_param_value filt_env[THYNS_ENV_PARAM_COUNT];
  union clap_param_value voice[THYNS_VOICE_PARAM_COUNT];
};

# define THYNS_PARAMS_VALUES_COUNT                                      \
  (sizeof (struct thyns_params) / sizeof (union clap_param_value))

struct thyns_ramp
{
  struct thyns_ramp *prev;
  struct thyns_ramp *next;

  float                   increment;
  union clap_param_value *target;
};

struct thyns_ramps
{
  struct thyns_ramp *ramps[THYNS_PARAMS_VALUES_COUNT];
};

static void
thyns_params_init(struct thyns_params *params)
{
  thyns_osc_params_init(params->osc1);
  thyns_osc_params_init(params->osc2);
  thyns_filt_params_init(params->filt);
  thyns_env_params_init(params->amp_env);
  thyns_env_params_init(params->filt_env);
  thyns_voice_params_init(params->voice);
}

#endif /* !PARAMS_H */
