// 1.13c D2Multi+0x14B70 -- stdcall message handler: calls 0xBE00, sets the screen state at 0x39F20 to 3, calls 0x148F0, then hides (D2Win 10097 with 0) the three controls at 0x39154, 0x39150 and 0x3915C. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_39f20;
__attribute__((weak)) uint32_t g_d2multi_39154;
__attribute__((weak)) uint32_t g_d2multi_39150;
__attribute__((weak)) uint32_t g_d2multi_3915c;
namespace d2multi {
uint32_t f14b70(GE& e, uint32_t a0)
{
    (void)a0;
    e.h(0xbe00);
    g_d2multi_39f20 = 3;
    e.h(0x148f0);
    e.h(0x7c1a, 0, g_d2multi_39154, 0);
    e.h(0x7c1a, 0, g_d2multi_39150, 0);
    e.h(0x7c1a, 0, g_d2multi_3915c, 0);
    return 1;
}
}
