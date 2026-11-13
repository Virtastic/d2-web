// 1.13c D2Game+0x40B20 -- monster-mode callback: with the unit's 0xB102 value v and monster row short +0x18 > 0, when row short +0x96 - row +0x38 > v > row +0x38 (signed)
// hands 0x3D1E0 (EAX = row +0x3C, EBX = unit; game, row +0x40, short +0x18, 5); then, when row +0x44 >= 0, 0xB24C(unit) is 0 and v <= row +0x48 + row +0x38, hands
// 0x3BDA0 (ESI = unit; game, row +0x44). Returns 0x3F320(game) with EBX = unit; 2 when the unit or monster row is missing.
// fastcall: ECX = game, EDX = unit
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f40b20(NE& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 2;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return 2;
    const uint32_t row = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!row) return 2;
    const int32_t v = (int32_t)e.imp(0xB102, unit);
    const int32_t s = (int16_t)e.rd16(row + 0x18);
    if (s > 0) {
        const int32_t base = (int32_t)e.rd32(row + 0x38);
        if (v < (int32_t)(int16_t)e.rd16(row + 0x96) - base && v > base)
            e.h(0x3D1E0, e.rd32(row + 0x3c), 0, 0, unit, 0, game, e.rd32(row + 0x40), (uint32_t)s, 5);
    }
    if ((int32_t)e.rd32(row + 0x44) >= 0 && !e.imp(0xB24C, unit) && v <= (int32_t)(e.rd32(row + 0x48) + e.rd32(row + 0x38)))
        e.h(0x3BDA0, 0, 0, 0, 0, unit, game, e.rd32(row + 0x44));
    return e.h(0x3F320, 0, 0, 0, unit, 0, game);
}
}
