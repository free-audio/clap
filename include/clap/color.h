#pragma once

#include "private/std.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_color {
   uint8_t alpha;
   uint8_t red;
   uint8_t green;
   uint8_t blue;
} clap_color_t;

#ifdef __cplusplus
}
#endif
