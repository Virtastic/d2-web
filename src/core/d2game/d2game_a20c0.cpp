// 1.13c D2Game+0xA20C0: esi = unit, edi = game. unit+0xC4 |= 0x40; #E07A0 (eax = unit, ecx = game); t = #A598(unit+0x2c); t != 0 (or t == unit) runs #DED30(game, unit, t), returns 1; else 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_a20c0(uint32_t unit, uint32_t game, const D2Seam* e)
{
    d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    e->call(0xE07A0, unit, game);
    const uint32_t t = e->call(0xA598, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    if (t != unit && !t) return 0;
    e->call(0xDED30, 0, 0, 0, 0, 0, game, unit, t);
    return 1;
}
