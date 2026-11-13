// 1.13c D2Game+0x79A10 -- unit mode-start dispatch (fastcall: ECX = game, EDX = unit, stack = two words, ret 8). The handler slot is
// table[unit ? *(unit+0x10) : 0] of the pointer table at D2Game+0x1079D0 (the unit's animation mode); an empty slot does nothing. A handler (same
// register and stack arguments) that returns 2 is followed by 0x79850(game, unit, a, b).
#include <cstdint>
#include "d2game_gpx.h"
namespace d2g {
void f79a10(GE& e, uint32_t game, uint32_t unit, uint32_t a, uint32_t b, uint32_t table)
{
    const uint32_t slot = table + 4u * (unit ? e.rd32(unit + 0x10) : 0u);
    if (e.rd32(slot) == 0) return;
    if (e.h(slotRva(e, slot), 0, game, unit, 0, 0, a, b) == 2) e.h(0x79850, 0, game, unit, 0, 0, a, b);
}
}
