// 1.13c D2Game+0x4EC30: esi = unit. True when a player-type unit passes the four callee gates (D2Common #10xxx 0x13B0(unit->+4, 8), 0xDFA90, #ADC0(unit, 0), 0x9F120(unit, 7)).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4ec30(uint32_t unit, const D2Seam* e)
{
    if (!unit || d2rd32(unit) != 1) return 0;
    if (!e->call(0x13B0, d2rd32(unit + 4), 0, 8)) return 0;
    if (!e->call(0xDFA90, unit)) return 0;
    if (!e->call(0xADC0, 0, 0, 0, 0, 0, unit, 0)) return 0;
    return e->call(0x9F120, unit, 0, 0, 0, 0, 7) != 0;
}
