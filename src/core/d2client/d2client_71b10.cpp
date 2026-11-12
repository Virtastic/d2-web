// 1.13c D2Client+0x71B10 -- No arguments. A non-zero D2Gfx ordinal 10043 answer is returned as is. Otherwise D2Gfx ordinal 10085 (1, 0, 0, 0) opens the frame (0 returns 0), ordinal 10021 (0) clears it, D2Win ordinal 10062 runs, then 0x71280 (1), 0x717B0, 0x71220 (the dword 0x11C010), 0x71740, 0x16A90, and D2Gfx ordinal 10040 ends the frame (a tail jump); its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c010;
namespace d2cl {
uint32_t f71b10(GE& e)
{
    const uint32_t busy = e.h(0xD0A2);
    if (busy) return busy;
    const uint32_t ok = e.h(0xD0A8, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    if (!ok) return 0;
    e.h(0xD090, 0, 0, 0, 0, 0, 0);
    e.h(0xD3A2);
    e.h(0x71280, 0, 0, 0, 0, 0, 1);
    e.h(0x717B0);
    e.h(0x71220, 0, 0, 0, 0, 0, g_d2client_11c010);
    e.h(0x71740);
    e.h(0x16A90);
    return e.h(0xD08A);
}
}
