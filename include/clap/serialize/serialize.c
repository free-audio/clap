#include <string.h>

static inline bool
clap_serializer_dict(struct clap_serializer *s)
{
  if (!s->out || s->out + 1 > s->out_end)
    return false;
  *s->out = CLAP_SERIALIZE_DICT;
  ++s->out;
  return true;
}

static inline bool
clap_serializer_array(struct clap_serializer *s)
{
  if (!s->out || s->out + 1 > s->out_end)
    return false;
  *s->out = CLAP_SERIALIZE_ARRAY;
  ++s->out;
  return true;
}

static inline bool
clap_serializer_end(struct clap_serializer *s)
{
  if (!s->out || s->out + 1 > s->out_end)
    return false;
  *s->out = CLAP_SERIALIZE_END;
  ++s->out;
  return true;
}

static inline bool
clap_serializer_bool(struct clap_serializer *s, bool value)
{
  if (!s->out || s->out + 2 > s->out_end)
    return false;
  s->out[0] = CLAP_SERIALIZE_BOOL;
  s->out[1] = value;
  s->out += 2;
  return true;
}

static inline void
clap_serializer_int32_priv(struct clap_serializer *s, int32_t value)
{
  s->out[1] = value >> 24;
  s->out[2] = value >> 16;
  s->out[3] = value >> 8;
  s->out[4] = value;
}

static inline bool
clap_serializer_int32(struct clap_serializer *s, int32_t value)
{
  if (!s->out || s->out + 5 >= s->out_end)
    return false;

  *s->out = CLAP_SERIALIZE_INT32;
  clap_serializer_int32_priv(s, value);
  s->out += 5;
  return true;
}

static inline bool
clap_serializer_float(struct clap_serializer *s, float value)
{
  if (!s->out || s->out + 5 >= s->out_end)
    return false;

  union {
    float   f;
    int32_t i;
  } u;

  u.f = value;
  *s->out = CLAP_SERIALIZE_FLOAT;
  clap_serializer_int32_priv(s, u.i);
  s->out += 5;
  return true;
}

static inline bool
clap_serializer_str(struct clap_serializer *s,
                    const char             *str,
                    uint32_t                len)
{
  if (!s->out || s->out + 5 + len >= s->out_end)
    return false;

  *s->out = CLAP_SERIALIZE_STR;
  clap_serializer_int32_priv(s, len);
  memcpy(s->out + 5, str, len);
  s->out += 5 + len;
  return true;
}

static inline bool
clap_deserialize(struct clap_deserializer *d)
{
  if (!d->in || !d->in_end || d->in >= d->in_end) {
    d->type = CLAP_SERIALIZE_EOB;
    return false;
  }

  d->type = *d->in;
  switch (d->type) {
  case CLAP_SERIALIZE_DICT:
    ++d->in;
    return true;

  case CLAP_SERIALIZE_ARRAY:
    ++d->in;
    return true;

  case CLAP_SERIALIZE_END:
    ++d->in;
    return true;

  case CLAP_SERIALIZE_BOOL:
    if (d->in + 5 > d->in_end) {
      d->type = CLAP_SERIALIZE_EOB;
      return false;
    }
    d->b   = d->in[1];
    d->in += 2;
    return true;

  case CLAP_SERIALIZE_INT32:
  case CLAP_SERIALIZE_FLOAT:
    if (d->in + 5 > d->in_end) {
      d->type = CLAP_SERIALIZE_EOB;
      return false;
    }
    d->i   = (d->in[1] << 24) | (d->in[2] << 16) | (d->in[3] << 8) | d->in[4];
    d->in += 5;
    return true;

  case CLAP_SERIALIZE_STR:
    if (d->in + 5 > d->in_end) {
      d->type = CLAP_SERIALIZE_EOB;
      return false;
    }
    d->slen = (d->in[1] << 24) | (d->in[2] << 16) | (d->in[3] << 8) | d->in[4];
    if (d->in + 5 + d->slen > d->in_end) {
      d->type = CLAP_SERIALIZE_EOB;
      return false;
    }
    d->s  = (const char *)d->in + 5;
    d->in = d->in + 5 + d->slen;
    return true;

  default:
    d->type = CLAP_SERIALIZE_ERROR;
    return false;
  }
}
