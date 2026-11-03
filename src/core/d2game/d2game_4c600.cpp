// 1.13c D2Game+0x4C600 skill srv (Fend family): ecx=game, edx=unit, stack skill,lvl (ret 8).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4c600(uint32_t game, uint32_t unit, uint32_t skill, uint32_t lvl, uint32_t tab, const D2Seam* e)
{
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)d2rd32(tab + 0xba0)) return 0;
    const uint32_t row = skill * 0x23c + d2rd32(tab + 0xb98);
    if (!row) return 0;
    const uint32_t X = e->call(0xAABA,0,0,0,0,0,unit); if (!X) return 0;
    const uint32_t B = e->call(0xA77E,0,0,0,0,0,unit) + 4;
    e->call(0xE07A0, unit, game);
    const uint32_t p = e->call(0xA598,0,0,0,0,0,d2rd32(unit+0x2c));
    uint32_t rec, v = 0;   // v: out-param slot of A0F40 (not written by the hooked callee)
    if (p == unit || p == 0) {
        rec = e->call(0xA0F40, 0x20003, 0xffffffffu, B, 0,0, game, unit, 0, 0);
        if (!rec) { e->call(0xAC94,0,0,0,0,0,X,0); return 1; }
    } else {
        rec = p;
        v = e->call(0xA0FC0, 0x20003, game, B, 0,0, game, unit, 0, 0);
    }
    const uint32_t m = e->call(0xAC9A,0,0,0,0,0,unit,d2rd32(row+0x138),skill,lvl);
    if ((int32_t)v >= (int32_t)m) v = m;
    e->call(0xAC94,0,0,0,0,0,X,v);
    e->call(0xACDC,0,0,0,0,0,X,d2rd32(rec));
    e->call(0xAD0C,0,0,0,0,0,X,d2rd32(rec+0xc));
    return 1;
}
