#ifndef OSC_H
# define OSC_H

# include "defs.h"

enum thyns_osc_waveform
{
  THYNS_OSC_NONE   = 0,
  THYNS_OSC_SQUARE = 1,
  THYNS_OSC_SAW    = 2,
  THYNS_OSC_TRI    = 3,
  THYNS_OSC_SINE   = 4,
};

struct thyns_osc
{
  uint32_t sr;    // sample rate
  double   pi_sr; // M_PI / sample_rate

  enum thyns_osc_waveform waveform;
  double                  pwm; // 0..1

  double tune;
  double freq;
  double angle_ramp;
  double angle;
  double phase;
};

static inline void
thyns_osc_init(struct thyns_osc *osc, uint32_t sr)
{
  osc->sr       = sr;
  osc->pi_sr    = M_PI / ((float)sr);
  osc->waveform = THYNS_OSC_SQUARE;
  osc->pwm      = 0.5;
  osc->freq     = 0;
  osc->angle    = 0;
  osc->phase    = 0;
  osc->tune     = 0;
}

static inline void
thyns_osc_set_freq(struct thyns_osc *osc, double freq)
{
  osc->freq = freq;
  osc->angle_ramp = 2 * osc->pi_sr * freq * pow(2, osc->tune);
}

static inline double
thyns_osc_step(struct thyns_osc *osc)
{
  osc->angle = fmod(osc->angle + osc->angle_ramp, 2 * M_PI);
  double angle = fmod(osc->angle + osc->phase, 2 * M_PI);

  switch (osc->waveform) {
  case THYNS_OSC_NONE:
    return 0;

  case THYNS_OSC_SQUARE:
    return angle < M_PI;

  case THYNS_OSC_SAW:
    return (2 * M_PI - angle) / (M_PI) - 1;

  case THYNS_OSC_TRI:
    if (angle < M_PI)
      return angle * 2.0 / M_PI - 1;
    return (2 * M_PI - angle) * 2.0 / M_PI - 1;

  case THYNS_OSC_SINE:
    return sin(angle);

  default:
    assert(false);
    return 0;
  }
}

#endif /* !OSC_H */
