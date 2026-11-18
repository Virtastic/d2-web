// 1.13c D2Client+0x20BF0 -- thiscall, ECX = unit (may be 0), one stack word (ret 4). With a non-zero argument 0xC5EC(unit, &arg, &local)
// rewrites the argument slot; a present unit with +0x10 mode 3 and a rewritten slot of 2 runs 0xC1C6(unit, 0xC, 0) and
// 0x20340(EAX = result >> 1 arithmetic, unit, 0xC3).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
void f20bf0(GE& e, uint32_t u, uint32_t arg)
{
    if (!arg) return;
    uint32_t slot = arg, local = 0;
    e.h(0xC5EC, 0, 0, 0, 0, 0, u, (uint32_t)(uintptr_t)&slot, (uint32_t)(uintptr_t)&local);
    if (!u || e.rd32(u + 0x10) != 3 || slot != 2) return;
    const uint32_t v = e.h(0xC1C6, 0, 0, 0, 0, 0, u, 0xC, 0);
    e.h(0x20340, (uint32_t)((int32_t)v >> 1), 0, 0, 0, 0, u, 0xC3);
}
}
