// 1.13c D2Game+0x4EE80: esi = unit, stack a0, a1 (ret 8). True when the unit passes #A514 (clear), 0x9F120(unit, 0x13), 0xDFA90 (clear), #A808(#A478(unit)) (clear) and 0xE0A60(unit, a1; a0) is nonzero.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4ee80(uint32_t unit, uint32_t a0, uint32_t a1, const D2Seam* e)
{
    if (!unit) return 0;
    if (e->call(0xA514, 0, 0, 0, 0, 0, unit)) return 0;
    if (!e->call(0x9F120, unit, 0, 0, 0, 0, 0x13)) return 0;
    if (e->call(0xDFA90, unit)) return 0;
    const uint32_t t = e->call(0xA478, 0, 0, 0, 0, 0, unit);
    if (e->call(0xA808, 0, 0, 0, 0, 0, t)) return 0;
    return e->call(0xE0A60, unit, a1, 0, 0, 0, a0) != 0;
}
