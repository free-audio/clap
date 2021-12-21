#pragma once

#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

typedef struct clap_color {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
   uint8_t alpha;
} clap_color_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif