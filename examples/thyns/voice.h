#ifndef VOICE_H
# define VOICE_H

# include "env.h"
# include "filt.h"
# include "osc.h"
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
  double            osc_mix;

  // filter
  struct thyns_filt filt;
  struct thyns_env  filt_env;
  double            filt_env_depth;

  // amp
  struct thyns_env  amp_env;
  double            amp;

  // voice parameters
  struct thyns_params params;
};

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
  voice->osc_mix = 0;

  // filter
  thyns_filt_init(&voice->filt, sr);
  voice->filt.cutoff = 4000;
  voice->filt.resonance = 1.5;
  thyns_env_init(&voice->filt_env);
  voice->filt_env_depth = 0.2;

  // amp
  thyns_env_init(&voice->amp_env);
  voice->amp = 0.2;
}

static inline void
thyns_voice_params_init(struct thyns_voice  *voice,
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
}

static inline void
thyns_voice_use_param(struct thyns_voice  *voice,
                      struct thyns_params *params,
                      uint32_t             index)
{
  uint32_t i = 0;

  if (index < i + THYNS_OSC_PARAM_COUNT) {
    voice->osc1.values[index - i] = params->values + index;
  }
  i += THYNS_OSC_PARAM_COUNT;

  if (index < i + THYNS_OSC_PARAM_COUNT) {
    voice->osc2.values[index - i] = params->values + index;
  }
  i += THYNS_OSC_PARAM_COUNT;
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
  double oscm = osc1 * (1 - voice->osc_mix) + osc2 * voice->osc_mix;
  double fenv   = thyns_env_step(&voice->filt_env) * voice->filt_env_depth;
  double cutoff = exp(log(voice->filt.cutoff) + fenv);
  thyns_filt_set_cutoff(&voice->filt, cutoff);
  double filtered = thyns_filt_step(&voice->filt, oscm);
  double amp = voice->amp * thyns_env_step(&voice->amp_env);

  return filtered * amp;
}

#endif /* !VOICE_H */
