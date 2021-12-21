#include <stdint.h>
#include <stdalign.h>

#if UINTPTR_MAX == UINT64_MAX
#   pragma pack(push, 8)
#elif UINTPTR_MAX == UINT32_MAX
#   pragma pack(push, 4)
#elif UINTPTR_MAX == UINT16_MAX
#   pragma pack(push, 2)
#endif