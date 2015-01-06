#ifndef PARAMS_H
# define PARAMS_H

struct thyns_params
{
  union clap_param_value values[0];
  union clap_param_value osc1[THYNS_OSC_PARAM_COUNT];
  union clap_param_value osc2[THYNS_OSC_PARAM_COUNT];
  union clap_param_value filt[THYNS_FILT_PARAM_COUNT];
};

static void
thyns_params_init(struct thyns_params *params)
{
  thyns_osc_params_init(params->osc1);
  thyns_osc_params_init(params->osc2);
  thyns_filt_params_init(params->filt);
}

#endif /* !PARAMS_H */
