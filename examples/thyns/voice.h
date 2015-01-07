#ifndef VOICE_H
# define VOICE_H

# include "env.h"
# include "filt.h"
# include "osc.h"

enum thyns_voice_param_index
{
  THYNS_VOICE_PARAM_OSC_MIX = 0,
  THYNS_VOICE_PARAM_FILT_ENV_DEPTH,
  THYNS_VOICE_PARAM_AMP,
  THYNS_VOICE_PARAM_COUNT,
};

static inline void
thyns_voice_params_init(union clap_param_value *values)
{
  values[THYNS_VOICE_PARAM_OSC_MIX].f        = 0.5;
  values[THYNS_VOICE_PARAM_FILT_ENV_DEPTH].f = 0.2;
  values[THYNS_VOICE_PARAM_AMP].f            = 0.2;
}

# include "params.h"

struct thyns_voice
{
  struct thyns_voice *prev;
  struct thyns_voice *next;

  uint32_t sr; // sample rate
  double   pi_sr; // M_PI / sample_rate

  uint8_t  key;
  float    freq;

  // osc part
  struct thyns_osc  osc1;
  struct thyns_osc  osc2;

  // filter
  struct thyns_filt filt;
  struct thyns_env  filt_env;

  // amp
  struct thyns_env  amp_env;

  union clap_param_value *values[THYNS_VOICE_PARAM_COUNT];

  // voice parameters
  struct thyns_params params;
};

static inline void
thyns_voice_param_info(uint32_t                 index,
                       union clap_param_value   value,
                       const char              *prefix,
                       struct clap_param       *param)
{
#define P(Dst, Args...) snprintf(Dst, sizeof (Dst), Args);

  switch (index) {
  case THYNS_VOICE_PARAM_OSC_MIX:
    P(param->id, "%s%s", prefix, "osc-mix");
    P(param->name, "%s", "osc mix");
    P(param->desc, "%s", "Oscillators mixer");
    P(param->display, "%f", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = 0;
    param->max.f = 1;
    param->scale = CLAP_PARAM_LINEAR;
    break;

  case THYNS_VOICE_PARAM_FILT_ENV_DEPTH:
    P(param->id, "%s%s", prefix, "filt-env-depth");
    P(param->name, "%s", "filter env depth");
    P(param->desc, "%s", "Filter's envelop");
    P(param->display, "%f", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = -4;
    param->max.f = 4;
    param->scale = CLAP_PARAM_LINEAR;
    break;

  case THYNS_VOICE_PARAM_AMP:
    P(param->id, "%s%s", prefix, "amp");
    P(param->name, "%s", "output volume");
    P(param->desc, "%s", "Output volume");
    P(param->display, "%f", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = -1;
    param->max.f = 1;
    param->scale = CLAP_PARAM_LINEAR;
    break;
  }
#undef P
}

static inline void
thyns_voice_init(struct thyns_voice *voice, uint32_t sr)
{
  voice->prev = NULL;
  voice->next = NULL;

  voice->sr    = sr;
  voice->pi_sr = M_PI / sr;

  // osc
  thyns_osc_init(&voice->osc1, sr);
  thyns_osc_init(&voice->osc2, sr);

  // filter
  thyns_filt_init(&voice->filt, sr);
  thyns_env_init(&voice->filt_env, sr);

  // amp
  thyns_env_init(&voice->amp_env, sr);
}

static inline void
thyns_voice_values_init(struct thyns_voice  *voice,
                        struct thyns_params *params)
{
  uint32_t off = 0;

  // osc1
  for (int i = 0; i < THYNS_OSC_PARAM_COUNT; ++i)
    voice->osc1.values[i] = params->values + off + i;
  off += THYNS_OSC_PARAM_COUNT;

  // osc2
  for (int i = 0; i < THYNS_OSC_PARAM_COUNT; ++i)
    voice->osc2.values[i] = params->values + off + i;
  off += THYNS_OSC_PARAM_COUNT;

  // filt
  for (int i = 0; i < THYNS_FILT_PARAM_COUNT; ++i)
    voice->filt.values[i] = params->values + off + i;
  off += THYNS_FILT_PARAM_COUNT;

  // amp_env
  for (int i = 0; i < THYNS_ENV_PARAM_COUNT; ++i)
    voice->amp_env.values[i] = params->values + off + i;
  off += THYNS_ENV_PARAM_COUNT;

  // filt_env
  for (int i = 0; i < THYNS_ENV_PARAM_COUNT; ++i)
    voice->filt_env.values[i] = params->values + off + i;
  off += THYNS_ENV_PARAM_COUNT;

  // voice
  for (int i = 0; i < THYNS_VOICE_PARAM_COUNT; ++i)
    voice->values[i] = params->values + off + i;
  off += THYNS_VOICE_PARAM_COUNT;
}

static inline void
thyns_voice_use_value(struct thyns_voice  *voice,
                      struct thyns_params *params,
                      uint32_t             index)
{
  uint32_t i = 0;

  if (index < i + THYNS_OSC_PARAM_COUNT) {
    voice->osc1.values[index - i] = params->values + index;
    return;
  }
  i += THYNS_OSC_PARAM_COUNT;

  if (index < i + THYNS_OSC_PARAM_COUNT) {
    voice->osc2.values[index - i] = params->values + index;
    return;
  }
  i += THYNS_OSC_PARAM_COUNT;

  if (index < i + THYNS_FILT_PARAM_COUNT) {
    voice->filt.values[index - i] = params->values + index;
    return;
  }
  i += THYNS_FILT_PARAM_COUNT;

  if (index < i + THYNS_ENV_PARAM_COUNT) {
    voice->amp_env.values[index - i] = params->values + index;
    return;
  }
  i += THYNS_ENV_PARAM_COUNT;

  if (index < i + THYNS_ENV_PARAM_COUNT) {
    voice->filt_env.values[index - i] = params->values + index;
    return;
  }
  i += THYNS_ENV_PARAM_COUNT;

  if (index < i + THYNS_VOICE_PARAM_COUNT) {
    voice->values[index - i] = params->values + index;
    return;
  }
  i += THYNS_ENV_PARAM_COUNT;
}

static inline void
thyns_voice_start_note(struct thyns_voice *voice,
                       uint32_t            key,
                       float               freq)
{
  thyns_osc_set_freq(&voice->osc1, freq);
  thyns_osc_set_freq(&voice->osc2, freq);

  thyns_env_restart(&voice->filt_env);
  thyns_env_restart(&voice->amp_env);
}

static inline void
thyns_voice_stop_note(struct thyns_voice *voice,
                      uint32_t            key)
{
  thyns_env_release(&voice->filt_env);
  thyns_env_release(&voice->amp_env);
}

static inline double
thyns_voice_step(struct thyns_voice *voice)
{
  double osc1 = thyns_osc_step(&voice->osc1);
  double osc2 = thyns_osc_step(&voice->osc2);
  double oscm = voice->values[THYNS_VOICE_PARAM_OSC_MIX]->f;
  double osc  = osc1 * (1 - oscm) + osc2 * oscm;
  double fenv   = thyns_env_step(&voice->filt_env) *
    voice->values[THYNS_VOICE_PARAM_FILT_ENV_DEPTH]->f;
  double cutoff = exp(log(voice->filt.values[THYNS_FILT_PARAM_CUTOFF]->f) + fenv);
  double filtered = thyns_filt_step(&voice->filt, osc, cutoff,
                                    voice->filt.values[THYNS_FILT_PARAM_RESONANCE]->f);
  double amp = voice->values[THYNS_VOICE_PARAM_AMP]->f
    * thyns_env_step(&voice->amp_env);

  return filtered * amp;
}

#endif /* !VOICE_H */
