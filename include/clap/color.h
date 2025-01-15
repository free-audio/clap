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

static const CLAP_CONSTEXPR clap_color_t CLAP_COLOR_TRANSPARENT = { 0, 0, 0, 0 };

#ifdef __cplusplus
}
#endif
