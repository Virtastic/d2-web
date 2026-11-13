// 1.13c D2Game+0x413D0 -- monster-mode callback: skill id 0xB26A(unit) selects a skill row (dt +0xB98, 0x23C stride, count dt +0xBA0); level 0xB0FC(unit); owner 0xE03A0 when unit flag
// bit 10 (else 0), looked up twice; x = 0xAC9A(owner, skill +0x138, skill, level), then 0xAC9A(owner, skill +0x13C, skill, level) (result left in EAX); 0x3D1E0 (EAX = that result,
// EBX = unit; game, x, monster row short +0x18, 5). Returns 0x3F320(game) with EBX = unit; 2 when the unit, monster row or skill row is missing.
// A missing unit or monster row falls into a short test at address 0x18 in retail and faults, so no vector reaches those paths.
// fastcall: ECX = game, EDX = unit
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f413d0(NE& e, uint32_t dt, uint32_t game, uint32_t unit)
{
    if (!unit) return 2;
    const int32_t id = (int32_t)e.rd32(unit + 4);
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xb6c)) return 2;
    const uint32_t mrow = e.rd32(dt + 0xb64) + (uint32_t)id * 0x1a4u;
    if (!mrow) return 2;
    const int32_t sid = (int32_t)e.imp(0xB26A, unit);
    if (sid < 0 || sid >= (int32_t)e.rd32(dt + 0xba0)) return 2;
    const uint32_t srow = (uint32_t)sid * 0x23cu + e.rd32(dt + 0xb98);
    if (!srow) return 2;
    const uint32_t lvl = e.imp(0xB0FC, unit);
    const uint32_t o1 = (e.rd32(unit + 0xc8) >> 10 & 1) ? e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98)) : 0;
    const uint32_t x = e.imp(0xAC9A, o1, e.rd32(srow + 0x138), (uint32_t)sid, lvl);
    const uint32_t o2 = (e.rd32(unit + 0xc8) >> 10 & 1) ? e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98)) : 0;
    const uint32_t y = e.imp(0xAC9A, o2, e.rd32(srow + 0x13c), (uint32_t)sid, lvl);
    e.h(0x3D1E0, y, 0, 0, unit, 0, game, x, (uint32_t)(int32_t)(int16_t)e.rd16(mrow + 0x18), 5);
    return e.h(0x3F320, 0, 0, 0, unit, 0, game);
}
}
