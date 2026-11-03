// 1.13c D2Game+0x4C870 skill srv (two-bound scale): ecx=game, edx=unit, stack skill,lvl (ret 8).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4c870(uint32_t game, uint32_t unit, uint32_t skill, uint32_t lvl, uint32_t tab, const D2Seam* e)
{
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)d2rd32(tab + 0xba0)) return 0;
    const uint32_t row = skill * 0x23c + d2rd32(tab + 0xb98);
    if (!row) return 0;
    const uint32_t X = e->call(0xAABA,0,0,0,0,0,unit); if (!X) return 0;
    if (!e->call(0x9EB80, unit)) {
        e->call(0x79100, unit, 0,0,0,0, game);
        e->call(0x78A90, 0,0,0,0,0, game);
        return 0;
    }
    const uint32_t z = e->call(0xAC9A,0,0,0,0,0,unit,d2rd32(row+0x64),skill,lvl);
    e->call(0xE07A0, unit, game);
    const uint32_t p = e->call(0xA598,0,0,0,0,0,d2rd32(unit+0x2c));
    uint32_t rec, l0 = 0;   // l0: A0F40 out-param slot (not written by the hooked callee)
    if (p == unit || p == 0) {
        rec = e->call(0xA0F40, 3, 0xffffffffu, z, 0,0, game, unit, 0, 0);
    } else {
        rec = p;
        l0 = e->call(0xA0FC0, 3, p, z, 0,0, game, unit, 0, 0);
    }
    int32_t z2 = (int32_t)e->call(0xAC9A,0,0,0,0,0,unit,d2rd32(row+0x140),skill,lvl);
    const int32_t w = (int32_t)e->call(0xAC9A,0,0,0,0,0,unit,d2rd32(row+0x138),skill,lvl);
    if (z2 > w) z2 = w;
    int32_t m = z2; if ((int32_t)l0 > m) m = (int32_t)l0;
    int32_t r = w; if (m < w) r = m;
    e->call(0xAC94,0,0,0,0,0,X,(uint32_t)r);
    if (rec) {
        e->call(0xACDC,0,0,0,0,0,X,d2rd32(rec));
        e->call(0xAD0C,0,0,0,0,0,X,d2rd32(rec+0xc));
    } else {
        e->call(0xACDC,0,0,0,0,0,X,1);
        e->call(0xAD0C,0,0,0,0,0,X,0xffffffffu);
    }
    e->call(0xA1790,0,0,0,0,0,game);
    return 1;
}
