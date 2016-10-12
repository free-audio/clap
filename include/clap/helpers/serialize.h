#ifndef CLAP_HELPERS_SERIALIZE_H
# define CLAP_HELPERS_SERIALIZE_H

# ifdef __cplusplus
extern "C" {
# endif

# include <stdint.h>

enum clap_serialize_type
{
  CLAP_SERIALIZE_EOB   = 'B',
  CLAP_SERIALIZE_ERROR = 'E',

  CLAP_SERIALIZE_DICT  = 'd',
  CLAP_SERIALIZE_ARRAY = 'a',
  CLAP_SERIALIZE_END   = 'e',
  CLAP_SERIALIZE_STR   = 's',
  CLAP_SERIALIZE_BOOL  = 'b',
  CLAP_SERIALIZE_INT8  = 'c',
  CLAP_SERIALIZE_INT16 = 'j',
  CLAP_SERIALIZE_INT32 = 'i',
  CLAP_SERIALIZE_INT64 = 'l',

  /* IEEE 754 format */
  CLAP_SERIALIZE_FLOAT = 'f',
  CLAP_SERIALIZE_DOUBLE = 'g',
};

struct clap_serializer
{
  /* output buffer */
  uint8_t *out;
  uint8_t *out_end;
};

/* Begins a dictionary. Returns false on EOB. */
static inline bool
clap_serializer_dict(struct clap_serializer *s);

/* Begins an array. Returns false on EOB. */
static inline bool
clap_serializer_array(struct clap_serializer *s);

/* Ends an array or dictionary. Returns false on EOB. */
static inline bool
clap_serializer_end(struct clap_serializer *s);

/* Pushes a string. Returns false on EOB.
 * The string might not be null terminated. */
static inline bool
clap_serializer_str(struct clap_serializer *s,
                    const char             *str,
                    int32_t                len);

/* Pushes an 8 bits integer. Returns false on EOB. */
static inline bool
clap_serializer_bool(struct clap_serializer *s, bool value);

/* Pushes an 8 bits integer. Returns false on EOB. */
static inline bool
clap_serializer_int8(struct clap_serializer *s, int16_t value);

/* Pushes an 16 bits integer. Returns false on EOB. */
static inline bool
clap_serializer_int16(struct clap_serializer *s, int16_t value);

/* Pushes an 32 bits integer. Returns false on EOB. */
static inline bool
clap_serializer_int32(struct clap_serializer *s, int32_t value);

/* Pushes an 64 bits integer. Returns false on EOB. */
static inline bool
clap_serializer_int64(struct clap_serializer *s, int64_t value);

/* Pushes a float. Returns false on EOB. */
static inline bool
clap_serializer_float(struct clap_serializer *s, float value);

/* Pushes a double. Returns false on EOB. */
static inline bool
clap_serializer_double(struct clap_serializer *s, double value);

struct clap_deserializer
{
  /* input buffer */
  const uint8_t *in;
  const uint8_t *in_end;

  /* decoded value */
  enum clap_serialize_type  type;
  union {
    bool                    b;
    int8_t                  i8;
    int16_t                 i16;
    int32_t                 i32;
    int64_t                 i64;
    float                   f;
    double                  d;
    const char             *s;
  };
  int32_t                  slen;
};

/* Decodes one token into *d.
 * Returns false if the type is: CLAP_SERIALIZE_{ERROR,EOB}.
 */
static inline bool
clap_deserialize(struct clap_deserializer *d);

# include "serialize.c"

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_HELPERS_SERIALIZE_H */
