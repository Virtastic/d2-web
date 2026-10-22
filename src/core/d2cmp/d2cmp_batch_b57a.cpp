#include <initializer_list>
#include <cstdint>

// CRT heap-mode globals (DAT_6fe34604 = OS platform id, DAT_6fe34610 = OS major version).
static uint32_t s_b57a_osplatform;
static uint32_t s_b57a_osmajor;

// 1.13c D2CMP+0x480B
extern "C" int32_t D2CMP_heap_select_480B(void)
{
    if (s_b57a_osplatform == 2 && 4 < s_b57a_osmajor) {
        return 1;
    }
    return 3;
}
