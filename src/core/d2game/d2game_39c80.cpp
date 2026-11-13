// 1.13c D2Game+0x39C80 -- monster-mode callback: sets bit 0x20000 of the stat list's flags (+0xC4) unless the unit's owner already has flag bit 9 set
// fastcall: ECX = game, EDX = unit, stack word 0 = stat list, stack word 1 unused (ret 8)
// Returns EAX at the ret: the stat list pointer after the store, 1 when the owner has bit 9, the unit itself when it lacks flag bit 10, 0 on every other exit.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f39c80(NE& e, uint32_t game, uint32_t unit, uint32_t list)
{
    if (!unit || !(e.rd32(unit + 0xc8) >> 10 & 1)) return unit;   // EAX = the unit (mov eax, edx) on this exit
    const uint32_t owner = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
    if (!owner) return 0;
    if (e.rd32(owner + 0xc4) >> 9 & 1) return 1;
    if (!list) return 0;
    e.wr32(list + 0xc4, e.rd32(list + 0xc4) | 0x20000u);
    return list;
}
}
