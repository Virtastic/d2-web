// 1.13c D2Client+0x20C40 -- ECX = unit (may be 0), EDX = event code, one stack word (ret 4). A zero argument with code 0x12 does nothing.
// Code 0x2A on an absent or mode-0 unit: 0xCB5C(0x5A, 0) (EBX), 0xC1C6(unit, 0xC, 0), 0xA1020(EAX = unit, 0x5A, -1, 0, that result).
// Then 0xC5EC(unit, &arg, &local) rewrites the argument slot; a unit with a non-zero +0x10 mode, or a slot other than 4, ends the
// call. Code 0xA: 0xC1C6, 0x1660 and 0x1630 (ECX = unit) feed 0xB0450(0x9B, r1630, r1660, 0, r_c1c6), ESI = unit. Code 0x12:
// 0xCB5C(0x77, 0), 0xC1C6, 0xA1020(EAX = unit, 0xC2, -1, 0, result).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
void f20c40(GE& e, uint32_t u, uint32_t code, uint32_t arg)
{
    if (!arg && code == 0x12) return;
    uint32_t slot = arg, local = 0;
    if (code == 0x2A && (!u || e.rd32(u + 0x10) == 0)) {
        e.h(0xCB5C, 0, 0, 0, 0, 0, 0x5A, 0);
        const uint32_t c = e.h(0xC1C6, 0, 0, 0, 0, 0, u, 0xC, 0);
        e.h(0xA1020, u, 0, 0, 0, 0, u, 0x5A, 0xFFFFFFFFu, 0, c);
    }
    e.h(0xC5EC, 0, 0, 0, 0, 0, u, (uint32_t)(uintptr_t)&slot, (uint32_t)(uintptr_t)&local);
    if (u && e.rd32(u + 0x10)) return;
    if (slot != 4) return;
    if (code == 0xA) {
        const uint32_t c = e.h(0xC1C6, 0, 0, 0, 0, 0, u, 0xC, 0);
        const uint32_t y = e.h(0x1660, 0, u, 0, 0, 0);
        const uint32_t x = e.h(0x1630, 0, u, 0, 0, 0);
        e.h(0xB0450, 0, 0, 0, 0, u, 0x9B, x, y, 0, c);
    } else if (code == 0x12) {
        e.h(0xCB5C, 0, 0, 0, 0, 0, 0x77, 0);
        const uint32_t c = e.h(0xC1C6, 0, 0, 0, 0, 0, u, 0xC, 0);
        e.h(0xA1020, u, 0, 0, 0, 0, u, 0xC2, 0xFFFFFFFFu, 0, c);
    }
}
}
