// 1.13c D2Game+0x4ECD0: esi = unit, stack a0, a1 (ret 8). Gate for a player/monster unit: type-1 units need 0x1560(unit->+4, 2) and a clear flag 0x20 on unit->+0x14->+0x16; then unit->+0xC4 bits 1,2,3 set, 0xDFA90 clear, then 0xE0A60(unit, a1; a0) != 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4ecd0(uint32_t unit, uint32_t a0, uint32_t a1, const D2Seam* e)
{
    if (!unit) return 0;
    const uint32_t type = d2rd32(unit);
    if (type == 1) {
        if (!e->call(0x1560, d2rd32(unit + 4), 0, 2)) return 0;
        const uint32_t p = d2rd32(unit + 0x14);
        if (p && (d2rd8(p + 0x16) & 0x20)) return 0;
    }
    const uint32_t f = d2rd32(unit + 0xC4);
    if (!((f >> 2) & 1) || !((f >> 3) & 1) || !((f >> 1) & 1)) return 0;
    if (e->call(0xDFA90, unit)) return 0;
    return e->call(0xE0A60, unit, a1, 0, 0, 0, a0) != 0;
}
