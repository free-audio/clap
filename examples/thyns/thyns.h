#ifndef THYNS_H
# define THYNS_H

# include "voice.h"
# include "dlist.h"
# include "params.h"

# define THYNS_VOICES_COUNT 8
# define THYNS_RAMPS_COUNT  32

struct thyns
{
  uint32_t sr;    // sample rate
  double   pi_sr; // M_PI / sample_rate

  uint64_t steady_time;

  struct thyns_voice *voices_singing; // list
  struct thyns_voice *voices_idle;    // list
  struct thyns_voice *voices_keys[0x80];

  struct thyns_ramp *ramping;    // list
  struct thyns_ramp *ramps_idle; // list

  struct thyns_params params;
  struct thyns_ramps  ramps;

  /* allocators buffer */
  struct thyns_voice  voices_buffer[THYNS_VOICES_COUNT];
  struct thyns_ramp   ramps_buffer[THYNS_RAMPS_COUNT];
};

static inline void thyns_init(struct thyns *thyns, uint32_t sr)
{
  memset(thyns, 0, sizeof (*thyns));

  thyns->sr      = sr;
  thyns->pi_sr   = M_PI / sr;

  thyns_params_init(&thyns->params);

  for (uint32_t i = 0; i < THYNS_VOICES_COUNT; ++i) {
    thyns_voice_init(thyns->voices_buffer + i, sr);
    thyns_dlist_push_back(thyns->voices_idle, thyns->voices_buffer + i);
  }

  for (uint32_t i = 0; i < THYNS_RAMPS_COUNT; ++i)
    thyns_dlist_push_back(thyns->ramps_idle, thyns->ramps_buffer + i);
}

static double thyns_step(struct thyns        *thyns,
                         struct clap_process *process)
{
  if (!thyns->voices_singing)
    return 0;

  double out = 0;
  struct thyns_voice *prev = NULL;
  struct thyns_voice *v    = thyns->voices_singing;
  struct thyns_voice *end  = v->prev;

  /* compute voices */
  do {
    out += thyns_voice_step(v);

    // can we release the voice?
    if (v->amp_env.state == THYNS_ENV_IDLE) {
      printf("releasing %d\n", v->key);
      assert(v->key != 0);
      thyns->voices_keys[v->key] = NULL;
      thyns_dlist_remove(thyns->voices_singing, v);
      thyns_dlist_push_back(thyns->voices_idle, v);
      v = prev ? prev->next : thyns->voices_singing;
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
  if (thyns->voices_keys[key]) {
    voice = thyns->voices_keys[key];
  } else {
    if (thyns->voices_idle) {
      voice = thyns->voices_idle;
      thyns_dlist_remove(thyns->voices_idle, voice);
    } else {
      voice = thyns->voices_singing;
      thyns_dlist_remove(thyns->voices_singing, voice);
      thyns->voices_keys[voice->key] = NULL;
    }
    thyns_dlist_push_back(thyns->voices_singing, voice);
    thyns->voices_keys[key] = voice;
    voice->key = key;
  }

  thyns_voice_values_init(voice, &thyns->params);
  thyns_voice_start_note(thyns->voices_keys[key], key, pitch);
}

static inline void
thyns_note_off(struct thyns *thyns,
               uint8_t       key)
{
  printf("note_off(%d)\n", key);
  assert(key < 0x80);
  struct thyns_voice *voice = thyns->voices_keys[key];
  if (voice) {
    thyns_voice_stop_note(voice, key);

    // stop ramps
    for (struct thyns_ramp *ramp = voice->ramping; ramp; ramp = voice->ramping) {
      thyns_dlist_remove(voice->ramping, ramp);
      thyns_dlist_push_back(thyns->ramps_idle, ramp);
    }
  }
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

  case CLAP_EVENT_PARAM_SET:
    if (ev->param.is_global) {
      thyns->params.values[ev->param.index] = ev->param.value;

      // stop ramps
      if (thyns->ramps.ramps[ev->param.index]) {
        thyns_dlist_remove(thyns->ramping, thyns->ramps.ramps[ev->param.index]);
        thyns->ramps.ramps[ev->param.index] = NULL;
      }
    } else if (thyns->voices_keys[ev->param.key]) {
      struct thyns_voice *voice = thyns->voices_keys[ev->param.key];
      voice->params.values[ev->param.index] = ev->param.value;
      thyns_voice_use_value(voice, &voice->params, ev->param.index);

      // stop ramps
      if (voice->ramps.ramps[ev->param.index]) {
        thyns_dlist_remove(voice->ramping, voice->ramps.ramps[ev->param.index]);
        voice->ramps.ramps[ev->param.index] = NULL;
      }
    }
    break;

  case CLAP_EVENT_PARAM_RAMP: {
    struct thyns_ramps  *ramps  = NULL;
    struct thyns_params *params = NULL;
    struct thyns_voice  *voice  = NULL;

    if (ev->param.is_global) {
      // check if the parameter is ramping
      if (thyns->ramps.ramps[ev->param.index]) {
        // just update the increment
        thyns->ramps.ramps[ev->param.index]->increment = ev->param.increment;
        break;
      }

      // do we have idle ramps?
      if (!thyns->ramps_idle)
        break;

      ramps  = &thyns->ramps;
      params = &thyns->params;
    } else {
      voice = thyns->voices_keys[ev->param.key];

      // check if the voice is singing
      if (!voice)
        break;

      // check if the parameter is ramping
      if (voice->ramps.ramps[ev->param.index]) {
        // just update the increment
        voice->ramps.ramps[ev->param.index]->increment = ev->param.increment;
        break;
      }

      // do we have idle ramps?
      if (!thyns->ramps_idle)
        break;

      ramps  = &voice->ramps;
      params = &voice->params;
    }

    struct thyns_ramp *ramp = thyns->ramps_idle;
    thyns_dlist_remove(thyns->ramps_idle, ramp);
    ramp->increment = ev->param.increment;
    ramp->target = params->values + ev->param.index;
    ramps->ramps[ev->param.index] = ramp;

    // add the ramp to the ramping list
    if (ev->param.is_global) {
      thyns_dlist_push_back(thyns->ramping, ramp);
    } else {
      thyns_dlist_push_back(voice->ramping, ramp);
    }
    break;
  }

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
    process->outputs[0][i] = thyns_step(thyns, process);
    process->outputs[1][i] = process->outputs[0][i];
  }

  // ensure no more events are left
  assert(!ev);

  if (thyns->voices_singing)
    return CLAP_PROCESS_CONTINUE;
  return CLAP_PROCESS_STOP;
}

#endif /* !THYNS_H */
