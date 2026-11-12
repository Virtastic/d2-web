// 1.13c D2Multi+0x15380 -- stdcall message handler: calls 0xBE00 and 0x14BC0, then hides (D2Win 10097 with 0) the three controls at 0x39154, 0x39158 and 0x3915C. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_39154;
__attribute__((weak)) uint32_t g_d2multi_39158;
__attribute__((weak)) uint32_t g_d2multi_3915c;
namespace d2multi {
uint32_t f15380(GE& e, uint32_t a0)
{
    (void)a0;
    e.h(0xbe00);
    e.h(0x14bc0);
    e.h(0x7c1a, 0, g_d2multi_39154, 0);
    e.h(0x7c1a, 0, g_d2multi_39158, 0);
    e.h(0x7c1a, 0, g_d2multi_3915c, 0);
    return 1;
}
}
