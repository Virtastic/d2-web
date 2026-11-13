// 1.13c D2Game+0x39BD0 -- monster-mode callback: stores the unit's age (0xB24C minus game +0xA8) in event record +0x2C when it is 10 or more and within the
// 0xACE2 limit for the unit's owner; otherwise (or with no owner or no stat list) zeroes record +0x28 and +0x2C
// fastcall: ECX = game, EDX = unit, stack word 0 = stat list (null fails), stack word 1 = event record (ret 8)
// Returns EAX at the ret: the 0xACE2 limit on the store path, the event record pointer on every other exit.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f39bd0(NE& e, uint32_t game, uint32_t unit, uint32_t list, uint32_t rec)
{
    uint32_t owner = 0;
    if (unit && (e.rd32(unit + 0xc8) >> 10 & 1)) owner = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
    if (owner && list) {
        const uint32_t a = e.imp(0xB26A, unit);
        const uint32_t b = e.imp(0xB0FC, unit);
        const int32_t age = (int32_t)(e.imp(0xB24C, unit) - e.rd32(game + 0xa8));
        if (age >= 10) {
            const int32_t lim = (int32_t)e.imp(0xACE2, owner, a, b, 1);
            if (age <= lim) { e.wr32(rec + 0x2c, (uint32_t)age); return (uint32_t)lim; }
        }
    }
    e.wr32(rec + 0x28, 0);
    e.wr32(rec + 0x2c, 0);
    return rec;
}
}
