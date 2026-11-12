// 1.13c D2Multi+0x8770 -- stdcall message handler: when the callback pointer at 0x3A0E0 is set, calls it with the message argument. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a0e0;
namespace d2multi {
uint32_t f8770(GE& e, uint32_t a0)
{
    const uint32_t fp = g_d2multi_3a0e0;
    if (fp) e.h(fp & 0xffffffu, 0, 0, 0, 0, 0, a0);
    return 1;
}
}
