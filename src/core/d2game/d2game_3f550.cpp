// 1.13c D2Game+0x3F550 -- monster-mode callback: for each of the three monster-row skill slots (shorts at row +0x18/+0x1A/+0x1C, trigger values at row +0x38/+0x3C/+0x40)
// with a positive id whose trigger equals 0xB28E(unit): gathers 0x1130, 0x1100, 0xB0FC, 0xB26A, hands 0xE0A20(unit; game) and the six-word set to 0xA0000;
// then returns 0x3F320(game). Missing unit, bad row index or null row returns 2.
// fastcall: ECX = game, EDX = unit
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f3f550(NE& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 2;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return 2;
    const uint32_t row = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!row) return 2;
    for (uint32_t k = 0; k < 3; k++) {
        const int32_t s = (int16_t)e.rd16(row + 0x18 + 2 * k);
        if (s <= 0) continue;
        if (e.imp(0xB28E, unit) != e.rd32(row + 0x38 + 4 * k)) continue;
        const uint32_t v30 = e.h(0x1130, 0, unit);
        const uint32_t v00 = e.h(0x1100, 0, unit);
        const uint32_t a = e.imp(0xB0FC, unit);
        const uint32_t b = e.imp(0xB26A, unit);
        const uint32_t esi = e.h(0xE0A20, unit, 0, 0, 0, 0, game);
        e.h(0xA0000, 0, 0, 0, 0, esi, game, b, a, (uint32_t)s, v00, v30);
    }
    return e.h(0x3F320, 0, 0, 0, 0, 0, game);   // regs `-`: EBX (the unit) is live in retail but not part of the record
}
}
