#ifndef THYNS_H
# define THYNS_H

# include <clap/clap.h>

# include "voice.h"

# define THYNS_VOICE_COUNT 32

struct thyns
{
  uint32_t sr; // sample rate
  double   pi_sr; // M_PI / sample_rate

  struct thyns_voice *running;
  struct thyns_voice *idle;

  struct thyns_voice buffer[THYNS_VOICE_COUNT];
};

static void thyns_init(struct thyns *thyns, uint32_t sr)
{
  thyns->sr      = sr;
  thyns->pi_sr   = M_PI / sr;
  thyns->running = NULL;
  thyns->idle    = thyns->buffer;

  for (uint32_t i = 0; i < THYNS_VOICE_COUNT; ++i) {
    thyns_voice_init(thyns->buffer + i, sr);
    thyns->buffer[i].next = thyns->buffer + i + 1;
  }
  thyns->buffer[THYNS_VOICE_COUNT - 1].next = NULL;
}

double thyns_step(struct thyns *thyns,
                  struct clap_process *process)
{
  double out = 0;
  struct thyns_voice *prev = NULL;
  struct thyns_voice *v    = thyns->running;

  while (v) {
    out += thyns_voice_step(v);

    // can we release the voice?
    if (v->amp_env.state == THYNS_ENV_IDLE) {
      if (prev) {
        prev->next  = v->next;
        v->next     = thyns->idle;
        thyns->idle = v;
        v           = prev->next;
      } else {
        v->next     = thyns->idle;
        thyns->idle = v;
        v           = thyns->running;
      }
    } else {
      prev = v;
      v    = v->next;
    }
  }

  return out;
}

#endif /* !THYNS_H */
