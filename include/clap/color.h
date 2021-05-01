#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clap_color {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
   uint8_t alpha;
} clap_color;

#ifdef __cplusplus
}
#endif