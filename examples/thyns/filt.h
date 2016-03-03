/**
 * Basic filter adapted from Urs Heckman's code:
 * http://www.kvraudio.com/forum/viewtopic.php?f=33&t=412944#p5766724
 */

#ifndef FILT_H
# define FILT_H

# include "defs.h"

enum thyns_filt_param_index
{
  THYNS_FILT_PARAM_CUTOFF = 0,
  THYNS_FILT_PARAM_RESONANCE,
  THYNS_FILT_PARAM_COUNT
};

struct thyns_filt
{
  uint32_t sr;    // sample rate
  double   pi_sr; // M_PI / sample_rate

  double g;
  double g_div;

  double iceq1;   // states in form
  double iceq2;   // of current
  double iceq3;   // equivalents
  double iceq4;   // of capacitors

  double y4;      // delayed feedback

  /* parameters */
  union clap_param_value *values[THYNS_FILT_PARAM_COUNT];
};

static inline void
thyns_filt_param_info(uint32_t                 index,
                      union clap_param_value   value,
                      const char              *prefix,
                      struct clap_param       *param)
{
#define P(Dst, Args...) snprintf(Dst, sizeof (Dst), Args);

  switch (index) {
  case THYNS_FILT_PARAM_CUTOFF:
    P(param->id, "%s%s", prefix, "cutoff");
    P(param->name, "%s", "cutoff");
    P(param->desc, "%s", "Filter's cutoff");
    P(param->display, "%f", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = 0;
    param->max.f = 28000;
    param->scale = CLAP_PARAM_SCALE_LOG;
    break;

  case THYNS_FILT_PARAM_RESONANCE:
    P(param->id, "%s%s", prefix, "resonance");
    P(param->name, "%s", "resonance");
    P(param->desc, "%s", "Filter's resonance");
    P(param->display, "%f", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = 0;
    param->max.f = 2;
    param->scale = CLAP_PARAM_SCALE_LINEAR;
    break;
  }
#undef P
}

static inline void
thyns_filt_params_init(union clap_param_value *values)
{
  values[THYNS_FILT_PARAM_CUTOFF].f    = 4000;
  values[THYNS_FILT_PARAM_RESONANCE].f = 1.5;
}

static inline void
thyns_filt_init(struct thyns_filt *filt, uint32_t sr)
{
  memset(filt, 0, sizeof (*filt));
  filt->sr    = sr;
  filt->pi_sr = M_PI / sr;
}

static inline double
thyns_filt_step(struct thyns_filt *filt,
                double             in,
                double             cutoff,
                double             resonance)
{
  filt->g     = tan(filt->pi_sr * cutoff);
  filt->g_div = 1.0 / (1.0 + filt->g);

  double x0 = in - resonance * filt->y4;
  double y1 = (filt->g * tanh(x0) + filt->iceq1) * filt->g_div;
  double y2 = (filt->g * tanh(y1) + filt->iceq2) * filt->g_div;
  double y3 = (filt->g * tanh(y2) + filt->iceq3) * filt->g_div;
  filt->y4  = (filt->g * tanh(y3) + filt->iceq4) * filt->g_div;

  filt->iceq1 = 2 * y1 - filt->iceq1;
  filt->iceq2 = 2 * y2 - filt->iceq2;
  filt->iceq3 = 2 * y3 - filt->iceq3;
  filt->iceq4 = 2 * filt->y4 - filt->iceq4;

  return filt->y4;
}

#endif /* !FILT_H */
