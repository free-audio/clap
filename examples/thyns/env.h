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

struct thyns_env
{
  // params
  uint32_t a; // duration in ms
  uint32_t d; // duration in ms
  double   s; // sustain level
  uint32_t r; // duration in ms

  // state
  enum thyns_env_state state;
  double ar; // attack ramp
  double dr; // decay ramp
  double rr; // release ramp
  double v;
};

static inline void thyns_env_init(struct thyns_env * restrict env)
{
  env->state = THYNS_ENV_IDLE;
}

static inline void thyns_env_restart(struct thyns_env * restrict env)
{
  env->state = THYNS_ENV_ATTACK;
  env->v     = 0;
}

static inline double thyns_env_step(struct thyns_env * restrict env)
{
  switch (env->state) {
  case THYNS_ENV_ATTACK:
    env->v += env->ar;
    if (env->v >= 1) {
      env->v = 1;
      env->state = THYNS_ENV_DECAY;
    }
    break;

  case THYNS_ENV_DECAY:
    env->v += env->dr;
    if (env->v <= env->s) {
      env->v = env->s;
      env->state = THYNS_ENV_SUSTAIN;
    }
    break;

  case THYNS_ENV_SUSTAIN:
    break;

  case THYNS_ENV_RELEASE:
    env->v += env->rr;
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
