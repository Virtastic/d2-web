// 1.13c D2Client+0x64310 -- Returns 0 while the sound flag 0x11C280 is clear; otherwise continues in D2Sound ordinal 10046 (a tail jump) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c280;
namespace d2cl {
uint32_t f64310(GE& e)
{
    if (!g_d2client_11c280) return 0;
    return e.h(0xd2d6);
}
}
