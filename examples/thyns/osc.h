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

static const char * const thyns_osc_waveform_name[] = {
  "none",
  "square",
  "saw",
  "tri",
  "sine",
};

enum thyns_osc_param_index
{
  THYNS_OSC_PARAM_WAVEFORM = 0,
  THYNS_OSC_PARAM_PHASE,
  THYNS_OSC_PARAM_PWM,
  THYNS_OSC_PARAM_TUNE,
  THYNS_OSC_PARAM_COUNT
};

struct thyns_osc_params
{
  enum thyns_osc_waveform waveform;
  double                  pwm;  // 0..1
  double                  phase;
  double                  tune;
};

struct thyns_osc
{
  /* processing constants */
  uint32_t sr;    // sample rate
  double   pi_sr; // M_PI / sample_rate

  /* oscillator state */
  double freq;
  double angle_ramp;
  double angle;

  /* parameters */
  enum thyns_osc_waveform *waveform;
  double                  *pwm;  // 0..1
  double                  *phase;
  double                  *tune;
};

static inline void
thyns_osc_param_set(struct thyns_osc_params *params,
                    uint32_t                 index,
                    union clap_param_value   value)
{
  switch (index) {
  case THYNS_OSC_PARAM_WAVEFORM:
    params->waveform = value.i;
    break;

  case THYNS_OSC_PARAM_PHASE:
    params->phase = value.f;
    break;

  case THYNS_OSC_PARAM_PWM:
    params->pwm = value.f;
    break;

  case THYNS_OSC_PARAM_TUNE:
    params->tune = value.f;
    break;
  }
}

static inline void
thyns_osc_param_use(struct thyns_osc *osc,
                    struct thyns_osc_params *params,
                    uint32_t index)
{
  switch (index) {
  case THYNS_OSC_PARAM_WAVEFORM:
    osc->waveform = &params->waveform;
    break;

  case THYNS_OSC_PARAM_PHASE:
    osc->phase = &params->phase;
    break;

  case THYNS_OSC_PARAM_PWM:
    osc->pwm = &params->pwm;
    break;

  case THYNS_OSC_PARAM_TUNE:
    osc->tune = &params->tune;
    break;
  }
}

static inline void
thyns_osc_param_info(struct thyns_osc_params *params,
                     uint32_t                 index,
                     struct clap_param       *param,
                     const char              *prefix)
{
#define P(Dst, Args...) snprintf(Dst, sizeof (Dst), Args);

  switch (index) {
  case THYNS_OSC_PARAM_WAVEFORM:
    P(param->id, "%s%s", prefix, "waveform");
    P(param->name, "%s", "waveform");
    P(param->desc, "%s", "Oscillator's waveform");
    P(param->display, "%s", thyns_osc_waveform_name[params->waveform]);
    param->type = CLAP_PARAM_ENUM;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value.i = params->waveform;
    param->min.i = 0;
    param->max.i = THYNS_OSC_SINE;
    param->scale = CLAP_PARAM_LINEAR;
    break;

  case THYNS_OSC_PARAM_PHASE:
    P(param->id, "%s%s", prefix, "phase");
    P(param->name, "%s", "phase");
    P(param->desc, "%s", "Oscillator's phase");
    P(param->display, "%f", params->phase);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = true;
    param->value.f = params->phase;
    param->min.f = 0;
    param->max.f = 2 * M_PI;
    param->scale = CLAP_PARAM_LINEAR;
    break;

  case THYNS_OSC_PARAM_PWM:
    P(param->id, "%s%s", prefix, "pwm");
    P(param->name, "%s", "pwm");
    P(param->desc, "%s", "Oscillator's pulse width modulation");
    P(param->display, "%f", params->pwm);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value.f = params->pwm;
    param->min.f = 0;
    param->max.f = 1;
    param->scale = CLAP_PARAM_LINEAR;
    break;

  case THYNS_OSC_PARAM_TUNE:
    P(param->id, "%s%s", prefix, "tune");
    P(param->name, "%s", "tune");
    P(param->desc, "%s", "Oscillator's tunning in semitones");
    P(param->display, "%f", params->pwm);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value.f = params->tune;
    param->min.f = -48;
    param->max.f = +48;
    param->scale = CLAP_PARAM_LINEAR;
    break;
  }

#undef P
}

static inline void
thyns_osc_params_init(struct thyns_osc_params *params)
{
  params->waveform = THYNS_OSC_SQUARE;
  params->pwm      = 0.5;
  params->phase    = 0;
  params->tune     = 0;
}

static inline void
thyns_osc_init(struct thyns_osc *osc, uint32_t sr)
{
  osc->sr       = sr;
  osc->pi_sr    = M_PI / ((float)sr);
  osc->freq     = 0;
  osc->angle    = 0;
}

static inline void
thyns_osc_set_freq(struct thyns_osc *osc, double freq)
{
  osc->freq = freq;
  osc->angle_ramp = 2 * osc->pi_sr * freq * pow(2, *osc->tune / 12.f);
}

static inline double
thyns_osc_step(struct thyns_osc *osc)
{
  osc->angle = fmod(osc->angle + osc->angle_ramp, 2 * M_PI);
  double angle = fmod(osc->angle + *osc->phase, 2 * M_PI);

  switch (*osc->waveform) {
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
