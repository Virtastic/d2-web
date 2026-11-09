// 1.13c D2Game+0xED540: build the player unit for a client from its save record. EDI = game; stack (ret 8): client, &outUnit.
// `dt` is *sgptDataTables (the monster-stat table at +0xBC4, count +0xBC8, 0xC4 bytes a row); the caller reads it from the
// import slot. A client of 0 reports through 0xA2DA / 0xA2C2 and ends the process (0x2A3C(-1)).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ed540(uint32_t game, uint32_t cl, uint32_t pOut, uint32_t dt, const D2Seam* e)
{
    if (!cl) {
        const uint32_t m = e->call(0xA2DA, 0, 0, 0, 0, 0, 0x206);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6FD19FF9u, m, 0x206);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        return 0;
    }
    d2wr32(cl + 0x3d4, d2rd32(cl + 0x3d4) | 1);
    const uint32_t cls = d2rd8(cl + 8);
    uint32_t u = 0;
    if (cls < 7) {
        const uint32_t gid = d2rd32(game + 0x1c);
        u = e->call(0xA6B8, 0, 0, 0, 0, 0, gid, 0);
        if (u) {
            d2wr32(u, 0);
            d2wr32(u + 4, cls);
            d2wr32(u + 8, gid);
            d2wr32(u + 0x80, game);
            uint32_t c8 = d2rd32(u + 0xc8);
            c8 = d2rd32(game + 0x70) ? (c8 | 0x2000000) : (c8 & 0xfdffffffu);
            d2wr32(u + 0xc8, c8);
            d2wr32(u + 0xc8, c8 | 0x4000000);
            const uint32_t a = e->call(0xA556, 0, 0, 0, 0, 0, 0);
            const uint32_t act = e->call(0xA56E, 0, 0, 0, 0, 0, a) & 0xff;
            const uint32_t c4 = d2rd32(u + 0xc4);
            d2wr8(u + 0x18, act);
            d2wr32(u + 0x1c, d2rd32(game + 0xbc + act * 4));
            d2wr32(u + 0x74, 0);
            d2wr32(u + 0xc4, c4 | 0x10);
            d2wr32(u + 0xd0, 0xb);
            uint32_t ctr = d2rd32(game + 0x90) + 1;
            if (ctr == 0xffffffffu) ctr = 1;
            d2wr32(game + 0x90, ctr);
            e->call(0x38620, game, 0, 0, 0, u, ctr);
            e->call(0xEFED0, game, 0, 0, u, 0, 0);
        }
    }
    e->call(0xEBA40, cl, u, 0, 0, 0, game);
    const uint32_t r7 = e->call(0x7B0A0, 0, 0, 0, 0, 0, game, u, cl);
    const uint32_t bl = d2rd8(cl + 0x1ac);
    e->call(0xEB2C0, (r7 & 0xffffff00u) | bl, u, 0, 0, 0, game);
    e->call(0x385C0, 0, 0, 0, 0, 0, u, game);
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, u);
    d2wr32(pd + 0x74, 0);
    d2wr32(pd + 0x70, 0);
    if (bl == 0 && u) {
        const int32_t idx = (int32_t)d2rd32(u + 4);
        if (idx >= 0 && idx < (int32_t)d2rd32(dt + 0xbc8)) {
            const uint32_t rec = d2rd32(dt + 0xbc4) + (uint32_t)idx * 0xc4;
            const uint32_t w = d2rd16(rec + 0xac);
            if (w) {
                const int32_t sw = (int16_t)w;
                if (e->call(0xA3F4, 0, u, (uint32_t)sw)) {
                    e->call(0xA2100, 0, u, 0, 0, 0, (uint32_t)sw, 0xffffffffu);
                    d2wr32(pd + 0x70, (uint32_t)sw);
                }
            }
        }
    }
    e->call(0x35090, 0, 0, 0, 0, 0, game, u, 1);
    d2wr32(pOut, u);
    return 0;
}
