// 1.13c D2Game+0x40BD0 -- monster-mode callback: period = max(1, monster row +0x3C); when 0xB28E(unit) < row +0x38 (signed) and a multiple of the period, with a = 0xB24C(unit),
// b = 0xB288(unit): lo = (a-b)/2, hi = (a+b)/2 (truncating); 0xAB2C(unit +0x2C, 0x1100(unit)+lo, 0x1130(unit)+hi), 0xAAF0(unit +0x2C, unit, 0), 0xA50E(unit, lo), 0xAF5E(unit, hi).
// Returns 0x3F320(game) with EBX = unit; 2 when the unit or monster row is missing.
// fastcall: ECX = game, EDX = unit
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f40bd0(NE& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 2;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return 2;
    const uint32_t row = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!row) return 2;
    int32_t per = (int32_t)e.rd32(row + 0x3c);
    if (per < 1) per = 1;
    const int32_t m = (int32_t)e.imp(0xB28E, unit);
    if (m < (int32_t)e.rd32(row + 0x38) && (per == -1 ? 0 : m % per) == 0) {
        const int32_t a = (int32_t)e.imp(0xB24C, unit);
        const int32_t b = (int32_t)e.imp(0xB288, unit);
        const int32_t lo = (int32_t)((uint32_t)(a - b) + ((uint32_t)(a - b) >> 31)) >> 1;
        const int32_t hi = (int32_t)((uint32_t)(a + b) + ((uint32_t)(a + b) >> 31)) >> 1;
        const uint32_t v30 = e.h(0x1130, 0, unit);
        const uint32_t v00 = e.h(0x1100, 0, unit);
        e.imp(0xAB2C, e.rd32(unit + 0x2c), v00 + (uint32_t)lo, v30 + (uint32_t)hi);
        e.imp(0xAAF0, e.rd32(unit + 0x2c), unit, 0);
        e.imp(0xA50E, unit, (uint32_t)lo);
        e.imp(0xAF5E, unit, (uint32_t)hi);
    }
    return e.h(0x3F320, 0, 0, 0, unit, 0, game);
}
}
