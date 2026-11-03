// 1.13c D2Game+0x96C00: eax = list holder, stack game, p2, p3 (ret 0xC). Walks the list at [holder] (node +0 flag bit 0 skip, +4 id, +0x14 next; a null game is fatal, untestable).
// A node whose id has a unit in the game's bucket (game +0x1320 + (id & 0x7F)*4, chain +0xE4, key +0xC) has its position taken as (type 2, 4, 5: [[u +0x2C] +0x10], [[u +0x2C] +0xC];
// otherwise 0xA3B8 / 0xA454 of [u +0x2C], 0 when that is null) and so does p2; 0xAE26(p2y, p2x, uy, ux) above p3 (signed) calls 0x97B50(ecx = game, edx = p2; id, 1).
#include "d2game_seam4e.h"
static void pos(const D2Seam* e, uint32_t u, uint32_t* x, uint32_t* y)
{
    const uint32_t t = d2rd32(u), s = d2rd32(u + 0x2C);
    if (t == 2 || (t > 3 && t <= 5)) { *x = d2rd32(s + 0x10); *y = d2rd32(s + 0xC); return; }
    *x = s ? e->call(0xA3B8, 0, 0, 0, 0, 0, s) : 0;
    *y = s ? e->call(0xA454, 0, 0, 0, 0, 0, s) : 0;
}
extern "C" void d2game_96c00(uint32_t holder, uint32_t game, uint32_t p2, uint32_t p3, const D2Seam* e)
{
    for (uint32_t n = d2rd32(holder); n;) {
        const uint32_t next = d2rd32(n + 0x14);
        if (!(d2rd8(n) & 1)) {
            const uint32_t id = d2rd32(n + 4);
            uint32_t u = d2rd32(game + 0x1320 + (id & 0x7F) * 4);
            while (u && d2rd32(u + 0xC) != id) u = d2rd32(u + 0xE4);
            if (u) {
                uint32_t ux, uy, px, py;
                pos(e, u, &ux, &uy);
                pos(e, p2, &px, &py);
                if ((int32_t)e->call(0xAE26, 0, 0, 0, 0, 0, py, px, uy, ux) > (int32_t)p3)
                    e->call(0x97B50, 0, game, p2, 0, 0, id, 1);
            }
        }
        n = next;
    }
}
