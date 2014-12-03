#ifndef PARAMS_H
# define PARAMS_H

struct thyns_params
{
  struct thyns_osc_params osc1;
  struct thyns_osc_params osc2;
};

static void
thyns_params_init(struct thyns_params *params)
{
  thyns_osc_params_init(&params->osc1);
  thyns_osc_params_init(&params->osc2);
}

static void
thyns_params_set(struct thyns_params    *params,
                 uint32_t                index,
                 union clap_param_value  value)
{
  if (index < THYNS_OSC_PARAM_COUNT) {
    thyns_osc_param_set(&params->osc1, index, value);
    return;
  }

  index -= THYNS_OSC_PARAM_COUNT;
  if (index < THYNS_OSC_PARAM_COUNT) {
    thyns_osc_param_set(&params->osc2, index, value);
    return;
  }
}


#endif /* !PARAMS_H */
