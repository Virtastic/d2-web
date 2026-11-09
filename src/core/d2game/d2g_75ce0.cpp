// 1.13c D2Game+0x75CE0 -- ECX = game, EDX = unit; returns 1 (2 when 0xB084 says so). Written from the 1.13c disassembly.
// With p = 0xAABA(unit): flags f = 0xA79C(p); f bit 0 runs 0xE07A0(EAX unit, ECX game) and 0xE1930(EAX unit, EDI game); a 2
// there stores f | 2 (0xA78A) and calls 0xA2060(ESI unit; game) once, then once more unless the unit is mode 0xE without
// +0xC4 bit 6 (other modes: unless byte +0x4E != 1); every other case calls it once. 0xB08A(unit) ends it.
// Without p: x = 0x74F20(EAX mode, ECX unit). x != 0: 0xE1930, 0xB08A(unit), 0xB084(unit) nonzero returns 2, and only a
// mode-0xE unit with bit 6 (other modes: +0x4E == 1) goes on. Then (or with x == 0) 0x75C60(EAX unit, EBX game; x) zero runs
// 0x77790(ESI unit; game), 0xE07A0, and o = 0xA598(unit+0x2C) other than unit and nonzero gets 0xDED30(game, unit, o).
#include "../fn/hookenv.h"
namespace d2g {
static bool f75ce0Gate(HookEnv& e, uint32_t unit)
{
    if (unit && e.rd32(unit + 0x10) == 0xE) return (e.rd32(unit + 0xC4) >> 6) & 1;
    return e.rd8(unit + 0x4E) == 1;
}
uint32_t f75ce0(HookEnv& e, uint32_t game, uint32_t unit)
{
    const uint32_t p = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (p) {
        const uint32_t f = e.h(0xA79C, 0, 0, 0, 0, 0, p);
        bool again = true;
        if (f & 1) {
            e.h(0xE07A0, unit, game);
            const uint32_t r = e.h(0xE1930, unit);
            e.edi(0xE1930, game);
            if (r == 2) {
                e.h(0xA78A, 0, 0, 0, 0, 0, p, f | 2);
                e.h(0xA2060, 0, 0, 0, 0, unit, game);
                again = f75ce0Gate(e, unit);
            }
        }
        if (again) e.h(0xA2060, 0, 0, 0, 0, unit, game);
        e.h(0xB08A, 0, 0, 0, 0, 0, unit);
        return 1;
    }
    const uint32_t x = e.h(0x74F20, unit ? e.rd32(unit + 0x10) : 0, unit);
    if (x) {
        e.h(0xE1930, unit);
        e.edi(0xE1930, game);
        e.h(0xB08A, 0, 0, 0, 0, 0, unit);
        if (e.h(0xB084, 0, 0, 0, 0, 0, unit)) return 2;
        if (!f75ce0Gate(e, unit)) return 1;
    }
    if (e.h(0x75C60, unit, 0, 0, game, 0, x)) return 1;
    e.h(0x77790, 0, 0, 0, 0, unit, game);
    e.h(0xE07A0, unit, game);
    const uint32_t o = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (o != unit && o) e.h(0xDED30, 0, 0, 0, 0, 0, game, unit, o);
    return 1;
}
}
