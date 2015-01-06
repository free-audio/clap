#ifndef ENV_H
# define ENV_H

# include "defs.h"

enum thyns_env_state
{
  THYNS_ENV_ATTACK  = 0,
  THYNS_ENV_DECAY   = 1,
  THYNS_ENV_SUSTAIN = 2,
  THYNS_ENV_RELEASE = 3,
  THYNS_ENV_IDLE    = 4,
};

enum thyns_env_param_index
{
  THYNS_ENV_PARAM_ATTACK = 0,
  THYNS_ENV_PARAM_DECAY,
  THYNS_ENV_PARAM_SUSTAIN,
  THYNS_ENV_PARAM_RELEASE,
  THYNS_ENV_PARAM_COUNT
};

struct thyns_env
{
  // state
  enum thyns_env_state state;
  double msr; // 1000 / sr
  double v;

  union clap_param_value *values[THYNS_ENV_PARAM_COUNT];
};

static inline void
thyns_env_param_info(uint32_t                 index,
                     union clap_param_value   value,
                     const char              *prefix,
                     struct clap_param       *param)
{
#define P(Dst, Args...) snprintf(Dst, sizeof (Dst), Args);

  switch (index) {
  case THYNS_ENV_PARAM_ATTACK:
    P(param->id, "%s%s", prefix, "attack");
    P(param->name, "%s", "attack");
    P(param->desc, "%s", "Envelope's attack in ms");
    P(param->display, "%f ms", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = 0;
    param->max.f = 10000;
    param->scale = CLAP_PARAM_LOG;
    break;

  case THYNS_ENV_PARAM_DECAY:
    P(param->id, "%s%s", prefix, "decay");
    P(param->name, "%s", "decay");
    P(param->desc, "%s", "Envelope's decay in ms");
    P(param->display, "%f ms", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = 0;
    param->max.f = 10000;
    param->scale = CLAP_PARAM_LOG;
    break;

  case THYNS_ENV_PARAM_SUSTAIN:
    P(param->id, "%s%s", prefix, "sustain");
    P(param->name, "%s", "sustain");
    P(param->desc, "%s", "Envelope's sustain");
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

  case THYNS_ENV_PARAM_RELEASE:
    P(param->id, "%s%s", prefix, "release");
    P(param->name, "%s", "release");
    P(param->desc, "%s", "Envelope's release in ms");
    P(param->display, "%f ms", value.f);
    param->type = CLAP_PARAM_FLOAT;
    param->is_per_note = true;
    param->is_used = true;
    param->is_periodic = false;
    param->value = value;
    param->min.f = 0;
    param->max.f = 10000;
    param->scale = CLAP_PARAM_LOG;
    break;
  }
#undef P
}

static inline void
thyns_env_params_init(union clap_param_value *values)
{
  values[THYNS_ENV_PARAM_ATTACK].f  = 100;
  values[THYNS_ENV_PARAM_DECAY].f   = 200;
  values[THYNS_ENV_PARAM_SUSTAIN].f = 0.7;
  values[THYNS_ENV_PARAM_RELEASE].f = 200;
}

static inline void
thyns_env_init(struct thyns_env * restrict env, double sr)
{
  env->state = THYNS_ENV_IDLE;
  env->msr   = 1000.0 / sr;
  env->v     = 0;
}

static inline void
thyns_env_restart(struct thyns_env * restrict env)
{
  env->state = THYNS_ENV_ATTACK;
}

static inline void
thyns_env_release(struct thyns_env * restrict env)
{
  env->state = THYNS_ENV_RELEASE;
}

static inline double
thyns_env_step(struct thyns_env * restrict env)
{
  switch (env->state) {
  case THYNS_ENV_ATTACK:
    env->v += env->values[THYNS_ENV_PARAM_ATTACK]->f * env->msr;
    if (env->v >= 1) {
      env->v = 1;
      env->state = THYNS_ENV_DECAY;
    }
    break;

  case THYNS_ENV_DECAY: {
    float s = env->values[THYNS_ENV_PARAM_SUSTAIN]->f;
    env->v -= env->values[THYNS_ENV_PARAM_DECAY]->f * env->msr;
    if (env->v <= s) {
      env->v = s;
      env->state = THYNS_ENV_SUSTAIN;
    }
    break;
  }

  case THYNS_ENV_SUSTAIN:
    env->v = env->values[THYNS_ENV_PARAM_SUSTAIN]->f;
    break;

  case THYNS_ENV_RELEASE:
    env->v -= env->values[THYNS_ENV_PARAM_RELEASE]->f * env->msr;
    if (env->v <= 0) {
      env->v = 0;
      env->state = THYNS_ENV_IDLE;
    }
    break;

  case THYNS_ENV_IDLE:
    break;

  default:
    assert(false);
    break;
  }

  return env->v;
}

#endif /* !ENV_H */
