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
  union clap_param_value *values[THYNS_OSC_PARAM_COUNT];
};

static inline void
thyns_osc_param_info(uint32_t                 index,
                     union clap_param_value   value,
                     const char              *prefix,
                     struct clap_param       *param)
{
#define P(Dst, Args...) snprintf(Dst, sizeof (Dst), Args);

  switch (index) {
  case THYNS_OSC_PARAM_WAVEFORM:
    P(param->id, "%s%s", prefix, "waveform");
    P(param->name, "%s", "waveform");
    P(param->desc, "%s", "Oscillator's waveform");
    P(param->display, "%s", thyns_osc_waveform_name[value.i]);
    param->type = CLAP_PARAM_ENUM;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.i = 0;
    param->max.i = THYNS_OSC_SINE;
    param->scale = CLAP_PARAM_SCALE_LINEAR;
    break;

  case THYNS_OSC_PARAM_PHASE:
    P(param->id, "%s%s", prefix, "phase");
    P(param->name, "%s", "phase");
    P(param->desc, "%s", "Oscillator's phase");
    P(param->display, "%f", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = true;
    param->value = value;
    param->min.f = 0;
    param->max.f = 2 * M_PI;
    param->scale = CLAP_PARAM_SCALE_LINEAR;
    break;

  case THYNS_OSC_PARAM_PWM:
    P(param->id, "%s%s", prefix, "pwm");
    P(param->name, "%s", "pwm");
    P(param->desc, "%s", "Oscillator's pulse width modulation");
    P(param->display, "%f", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = 0;
    param->max.f = 1;
    param->scale = CLAP_PARAM_SCALE_LINEAR;
    break;

  case THYNS_OSC_PARAM_TUNE:
    P(param->id, "%s%s", prefix, "tune");
    P(param->name, "%s", "tune");
    P(param->desc, "%s", "Oscillator's tunning in semitones");
    P(param->display, "%f", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = -48;
    param->max.f = +48;
    param->scale = CLAP_PARAM_SCALE_LINEAR;
    break;
  }

#undef P
}

static inline void
thyns_osc_params_init(union clap_param_value *values)
{
  values[THYNS_OSC_PARAM_WAVEFORM].i = THYNS_OSC_SQUARE;
  values[THYNS_OSC_PARAM_PWM].f      = 0.5;
  values[THYNS_OSC_PARAM_PHASE].f    = 0;
  values[THYNS_OSC_PARAM_TUNE].f     = 0;
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
  osc->angle_ramp = 2 * osc->pi_sr * freq * pow(2, osc->values[THYNS_OSC_PARAM_TUNE]->f / 12.f);
}

static inline double
thyns_osc_step(struct thyns_osc *osc)
{
  osc->angle = fmod(osc->angle + osc->angle_ramp, 2 * M_PI);
  double angle = fmod(osc->angle + osc->values[THYNS_OSC_PARAM_PHASE]->f, 2 * M_PI);

  switch (osc->values[THYNS_OSC_PARAM_WAVEFORM]->i) {
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
