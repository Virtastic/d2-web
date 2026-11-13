// 1.13c D2Game+0x3AAA0 -- monster-mode callback: for a stat list whose first word is 1, raises event record +8 by the owner's 0xA42A(owner, 0x19, 0) percent
// (signed truncating divide by 100) when the unit has flag bit 10 and an owner (0xE03A0), then calls 0x3A790(game, unit, list, record)
// fastcall: ECX = game, EDX = unit, stack word 0 = stat list, stack word 1 = event record (ret 8)
// Returns EAX at the ret: 0 for a null list, the list's first word when it is not 1, else the 0x3A790 result.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f3aaa0(NE& e, uint32_t game, uint32_t unit, uint32_t list, uint32_t rec)
{
    if (!list) return 0;
    if (e.rd32(list) != 1) return e.rd32(list);
    if (unit && (e.rd32(unit + 0xc8) >> 10 & 1)) {
        const uint32_t owner = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
        if (owner) {
            const int32_t cur = (int32_t)e.rd32(rec + 8);
            const int32_t v = (int32_t)e.imp(0xA42A, owner, 0x19, 0);
            e.wr32(rec + 8, (uint32_t)((int32_t)((int64_t)(int32_t)((uint32_t)v * (uint32_t)cur) / 100) + cur));
        }
    }
    return e.h(0x3A790, 0, game, unit, 0, 0, list, rec);
}
}
