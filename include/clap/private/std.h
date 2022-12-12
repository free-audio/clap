#pragma once

#include "macros.h"

#ifdef CLAP_HAS_CXX11
#   include <cstdint>
#else
#   include <stdint.h>
#endif

#ifdef __cplusplus
#   include <cstddef>
#else
#   include <stddef.h>
#   include <stdbool.h>
#endif
