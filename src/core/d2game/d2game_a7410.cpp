// 1.13c D2Game+0xA7410: A7410 item-drop/stack handler.
// it = B17B0(unit, callback 0x6FCC39E0 | eax 1, edx game); 0 returns after F22C0(game, 0x19). cnt = AB86(unit, it); if the limit short at ls+0x56+idx*2 (p3+0x1c, idx = game byte 0x6D) <= cnt: ct = (unit type 1 and unit+0x14) ? [unit+0x14]+0x2C : 0; ct+0x18 = 7; F2540(game, unit, it, 2, 0, 0, 1). Else 598B0(game), A3CA(unit, 0x92, 1), AB50(unit+0x2C), 8E6F0 per type-2 node of unit+0xDC, A478/A44E/A550/A63A, E20A0(game, unit).
#include "d2game_seam4e.h"
extern "C" void d2g_a7410(uint32_t game, uint32_t unit, uint32_t p3, const D2Seam* e)
{
    const uint32_t ls = d2rd32(p3 + 0x1C);
    const uint32_t it = e->call(0xB17B0, 1, 0, game, 0, 0, unit, 0x6FCC39E0);
    if (!it) { e->call(0xF22C0, game, 0, 0, 0, 0, 0x19); return; }
    const int32_t cnt = (int32_t)e->call(0xAB86, 0, 0, 0, 0, 0, unit, it);
    const int32_t lim = (int16_t)d2rd16(ls + 0x56 + d2rd8(game + 0x6D) * 2);
    if (lim <= cnt) {
        uint32_t ct = 0;
        if (unit && d2rd32(unit) == 1 && d2rd32(unit + 0x14)) ct = d2rd32(d2rd32(unit + 0x14) + 0x2C);
        d2wr32(ct + 0x18, 7);
        e->call(0xF2540, 0, game, unit, 0, 0, it, 2, 0, 0, 1);
        return;
    }
    e->call(0x598B0, game);
    e->call(0xA3CA, 0, 0, 0, 0, 0, unit, 0x92, 1);
    e->call(0xAB50, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    for (uint32_t n = d2rd32(unit + 0xDC); n;) {
        const uint32_t nx = d2rd32(n + 0x24);
        if (d2rd8(n) == 2) e->call(0x8E6F0, n);
        n = nx;
    }
    const uint32_t v = e->call(0xA478, 0, 0, 0, 0, 0, unit, d2rd32(unit), d2rd32(unit + 0xC));
    e->call(0xA44E, 0, 0, 0, 0, 0, v);
    e->call(0xA550, 0, 0, 0, 0, 0, unit);
    e->call(0xA63A, 0, 0, 0, 0, 0, unit);
    e->call(0xE20A0, 0, game, unit);
}
