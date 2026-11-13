// 1.13c D2Game+0x40E10 -- monster-mode callback: period = max(1, monster row +0x38); when 0xB28E(unit) is a multiple of it, s = 0xB26A(unit), l = 0xB0FC(unit): s <= 0 or l <= 0
// returns 2 at once, else 0x9E420 (EAX = row +0x3C, ECX = l, EDX = s; game, unit) runs. Returns 0x3F320(game) with EBX = unit; 2 when the unit or monster row is missing.
// fastcall: ECX = game, EDX = unit
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f40e10(NE& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 2;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return 2;
    const uint32_t row = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!row) return 2;
    int32_t per = (int32_t)e.rd32(row + 0x38);
    if (per < 1) per = 1;
    const int32_t m = (int32_t)e.imp(0xB28E, unit);
    if ((per == -1 ? 0 : m % per) == 0) {
        const int32_t s = (int32_t)e.imp(0xB26A, unit);
        const int32_t l = (int32_t)e.imp(0xB0FC, unit);
        if (s <= 0 || l <= 0) return 2;
        e.h(0x9E420, e.rd32(row + 0x3c), (uint32_t)l, (uint32_t)s, 0, 0, game, unit);
    }
    return e.h(0x3F320, 0, 0, 0, unit, 0, game);
}
}
