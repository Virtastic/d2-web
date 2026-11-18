// 1.13c D2Client+0x20BC0 -- thiscall, ECX = unit (may be 0), one stack word (ret 4). With a non-zero argument and a unit that is absent or
// whose +0x10 mode is not 3: 0xC1C6(unit, 0xC, 0), then 0x20340(EAX = result >> 1 arithmetic, unit, 0xC3).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
void f20bc0(GE& e, uint32_t u, uint32_t arg)
{
    if (!arg) return;
    if (u && e.rd32(u + 0x10) == 3) return;
    const uint32_t v = e.h(0xC1C6, 0, 0, 0, 0, 0, u, 0xC, 0);
    e.h(0x20340, (uint32_t)((int32_t)v >> 1), 0, 0, 0, 0, u, 0xC3);
}
}
