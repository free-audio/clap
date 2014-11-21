#ifndef THYNS_H
# define THYNS_H

# include <clap/clap.h>

# include "voice.h"
# include "dlist.h"

# define THYNS_VOICE_COUNT 32

struct thyns
{
  uint32_t sr;    // sample rate
  double   pi_sr; // M_PI / sample_rate

  uint64_t steady_time;

  struct thyns_voice *singing;
  struct thyns_voice *idle;
  struct thyns_voice *keys[0x80];

  struct thyns_voice buffer[THYNS_VOICE_COUNT];
};

static inline void thyns_init(struct thyns *thyns, uint32_t sr)
{
  memset(thyns, 0, sizeof (*thyns));

  thyns->sr      = sr;
  thyns->pi_sr   = M_PI / sr;

  for (uint32_t i = 0; i < THYNS_VOICE_COUNT; ++i) {
    thyns_voice_init(thyns->buffer + i, sr);
    thyns_dlist_push_back(thyns->idle, thyns->buffer + i);
  }
}

static double thyns_step(struct thyns        *thyns,
                         struct clap_process *process)
{
  if (!thyns->singing)
    return 0;

  double out = 0;
  struct thyns_voice *prev = NULL;
  struct thyns_voice *v    = thyns->singing;
  struct thyns_voice *end  = v->prev;

  do {
    out += thyns_voice_step(v);

    // can we release the voice?
    if (v->amp_env.state == THYNS_ENV_IDLE) {
      printf("releasing %d\n", v->key);
      assert(v->key != 0);
      thyns->keys[v->key] = NULL;
      thyns_dlist_remove(thyns->singing, v);
      thyns_dlist_push_back(thyns->idle, v);
      v = prev ? prev->next : thyns->singing;
    } else {
      prev = v;
      v    = v->next;
    }
  } while (v && prev != end && v != prev);

  return out;
}

static inline void
thyns_note_on(struct thyns *thyns,
              uint8_t       key,
              float         pitch)
{
  struct thyns_voice *voice = NULL;

  printf("note_on(%d, %f)\n", key, pitch);
  assert(key < 0x80);
  if (thyns->keys[key]) {
    voice = thyns->keys[key];
  } else {
    if (thyns->idle) {
      voice = thyns->idle;
      thyns_dlist_remove(thyns->idle, voice);
    } else {
      voice = thyns->singing;
      thyns_dlist_remove(thyns->singing, voice);
      thyns->keys[voice->key] = NULL;
    }
    thyns_dlist_push_back(thyns->singing, voice);
    thyns->keys[key] = voice;
    voice->key = key;
  }

  thyns_voice_start_note(thyns->keys[key], key, pitch);
}

static inline void
thyns_note_off(struct thyns *thyns,
               uint8_t       key)
{
  printf("note_off(%d)\n", key);
  assert(key < 0x80);
  if (thyns->keys[key])
    thyns_voice_stop_note(thyns->keys[key], key);
}

static inline void
thyns_handle_event(struct thyns      *thyns,
                   struct clap_event *ev)
{
  switch (ev->type) {
  case CLAP_EVENT_NOTE_ON:
    thyns_note_on(thyns, ev->note.key, ev->note.pitch);
    break;

  case CLAP_EVENT_NOTE_OFF:
    thyns_note_off(thyns, ev->note.key);
    break;

  default:
    break;
  }
}

static inline enum clap_process_status
thyns_process(struct thyns *thyns, struct clap_process *process)
{
  struct clap_event *ev = process->events;

  thyns->steady_time = process->steady_time;
  for (uint32_t i = 0; i < process->samples_count; ++i, ++thyns->steady_time) {

    // handle events
    for (; ev; ev = ev->next) {
      assert(ev->steady_time >= thyns->steady_time);
      if (ev->steady_time > thyns->steady_time)
        break;

      thyns_handle_event(thyns, ev);
    }

    // process
    process->output[0][i] = thyns_step(thyns, process);
    process->output[1][i] = process->output[0][i];
  }

  // ensure no more events are left
  assert(!ev);

  if (thyns->singing)
    return CLAP_PROCESS_CONTINUE;
  return CLAP_PROCESS_STOP;
}

#endif /* !THYNS_H */
