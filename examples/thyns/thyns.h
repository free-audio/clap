#ifndef THYNS_H
# define THYNS_H

# include "env.h"
# include "filt.h"
# include "osc.h"

struct thyns
{
  uint32_t sr; // sample rate
  double   pi_sr; // M_PI / sample_rate

  // osc part
  struct thyns_osc  osc1;
  struct thyns_osc  osc2;
  double            osc_mix;

  // filter
  struct thyns_filt filt;
  struct thyns_env  filt_env;
  double            filt_env_depth;

  // amp
  struct thyns_env  amp_env;
  double            amp;
};

static void thyns_init(struct thyns *thyns, uint32_t sr)
{
  thyns->sr    = sr;
  thyns->pi_sr = M_PI / sr;

  // osc
  thyns_osc_init(&thyns->osc1);
  thyns_osc_init(&thyns->osc2);
  thyns->osc_mix = 0;

  // filter
  thyns_filt_init(&thyns->filt);
  thyns_filt_set_cutoff(&thyns->filt, 1000, thyns->pi_sr);
  thyns_env_init(&thyns->filt_env);
  thyns->filt_env_depth = 0;

  // amp
  thyns_env_init(&thyns->amp_env);
  thyns->amp = 0.7;
}

double thyns_step(struct thyns *thyns)
{
  double osc1 = thyns_osc_step(&thyns->osc1);
  double osc2 = thyns_osc_step(&thyns->osc2);
  double oscm = osc1 * (1 - thyns->osc_mix) + osc2 * thyns->osc_mix;

  double fenv   = thyns_env_step(&thyns->filt_env);
  double cutoff = exp(log(thyns->filt.cutoff) + fenv);
  thyns_filt_set_cutoff(&thyns->filt, cutoff, thyns->pi_sr);
  double filtered = thyns_filt_step(&thyns->filt, oscm);

  return filtered * thyns->amp * thyns_env_step(&thyns->amp_env);
}

#endif /* !THYNS_H */
