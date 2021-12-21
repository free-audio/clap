#pragma once

#include "std.h"

#if UINTPTR_MAX == UINT64_MAX
#   define CLAP_PTR_ALIGN 8
#elif UINTPTR_MAX == UINT32_MAX
#   define CLAP_PTR_ALIGN 4
#elif UINTPTR_MAX == UINT16_MAX
#   define CLAP_PTR_ALIGN 2
#endif

#define CLAP_ALIGN CLAP_PTR_ALIGN