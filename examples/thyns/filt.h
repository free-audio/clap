/**
 * Basic filter adapted from Urs Heckman's code:
 * http://www.kvraudio.com/forum/viewtopic.php?f=33&t=412944#p5766724
 */

#ifndef FILT_H
# define FILT_H

# include "defs.h"

struct thyns_filt
{
  uint32_t sr;    // sample rate
  double   pi_sr; // M_PI / sample_rate

  double cutoff; // in hz
  double resonance;

  double g;
  double g_div;

  float iceq1;   // states in form
  float iceq2;   // of current
  float iceq3;   // equivalents
  float iceq4;   // of capacitors

  float y4;      // delayed feedback
};

static inline void
thyns_filt_init(struct thyns_filt *filt, uint32_t sr)
{
  memset(filt, 0, sizeof (*filt));
  filt->sr    = sr;
  filt->pi_sr = M_PI / sr;
}

static inline void
thyns_filt_set_cutoff(struct thyns_filt *filt, double cutoff)
{
  filt->g     = tan(filt->pi_sr * cutoff);
  filt->g_div = 1.0 / (1.0 + filt->g);
}

static inline double
thyns_filt_step(struct thyns_filt *filt, double in)
{
  double x0 = in - filt->resonance * filt->y4;
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
