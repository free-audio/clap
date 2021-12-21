#include <stdint.h>
#include <stdalign.h>

#if UINTPTR_MAX == UINT64_MAX
#   define CLAP_PTR_ALIGN 8
#elif UINTPTR_MAX == UINT32_MAX
#   define CLAP_PTR_ALIGN 4
#elif UINTPTR_MAX == UINT16_MAX
#   define CLAP_PTR_ALIGN 2
#endif