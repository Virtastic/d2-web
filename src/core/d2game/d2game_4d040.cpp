// 1.13c D2Game+0x4D040 missile skill (mirrors the target through the caster): ecx=game, edx=unit, stack skill,lvl (ret 8).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4d040(uint32_t game, uint32_t unit, uint32_t skill, uint32_t lvl, uint32_t tab, const D2Seam* e)
{
    if (unit) d2wr32(unit + 0xc4, d2rd32(unit + 0xc4) | 0x40);
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)d2rd32(tab + 0xba0)) return 0;
    const uint32_t row = skill * 0x23c + d2rd32(tab + 0xb98);
    if (!row) return 0;
    e->call(0xE07A0, unit, game);
    const uint32_t p = e->call(0xA598,0,0,0,0,0,d2rd32(unit+0x2c));
    if (p == unit || p == 0) return 0;
    e->call(0xDED30,0,0,0,0,0,game,unit,p);
    const int32_t v = (int32_t)e->call(0xAC9A,0,0,0,0,0,unit,d2rd32(row+0x138),skill,lvl);
    auto st = [](uint32_t u) { const uint32_t t = d2rd32(u); return t == 2 || (t > 3 && t <= 5); };
    auto fx = [&](uint32_t u) { if (st(u)) return d2rd32(d2rd32(u+0x2c)+0xc); const uint32_t q = d2rd32(u+0x2c); return q ? e->call(0xA454,0,0,0,0,0,q) : 0u; };
    auto fy = [&](uint32_t u) { if (st(u)) return d2rd32(d2rd32(u+0x2c)+0x10); const uint32_t q = d2rd32(u+0x2c); return q ? e->call(0xA3B8,0,0,0,0,0,q) : 0u; };
    const uint32_t px = fx(p), py = fy(p);
    const uint32_t qx = fx(unit), qy = fy(unit);
    const int32_t m = (int32_t)e->call(0x91340, skill, 0,0,0,0, unit);
    if (m < 0 || m >= (int32_t)d2rd32(tab + 0xb6c)) return 0;
    uint32_t rq[23] = {};
    rq[0] = 0x21; rq[1] = unit; rq[4] = (uint32_t)m; rq[5] = px; rq[6] = py;
    rq[7] = 2 * px - qx; rq[8] = 2 * py - qy; rq[11] = skill; rq[12] = lvl; rq[21] = 0xB2000000u | 0x41F50u;  // the recorder tags a module code pointer 0xB2<rva>
    for (int32_t i = 0; i < v; i++) {
        rq[22] = (uint32_t)i;
        e->call(0x6F930, 0, game, (uint32_t)(uintptr_t)rq);
        e->blk(0x6F930, rq, 23);
    }
    return 1;
}
