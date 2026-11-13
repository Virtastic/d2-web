// 1.13c D2Game+0xBBDC0 -- quest 0x1A timer: a counting record spawns the quest unit beside its anchor after ten ticks; a waiting one drops the selected client after 95 s, runs the warning step after 90 s and the first notice after 75 s
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fbbdc0(NE& e, uint32_t eax0, uint32_t p0, uint32_t q)
{
    const uint32_t data = e.rd32(q + 0x18);
    if (e.rd8(data + 2)) {
        const uint32_t c = e.rd32(data + 0x18) + 1;
        e.wr32(data + 0x18, c);
        if (c < 10 || !e.rd8(data + 6)) return NE::lo(c, 0);
        const uint32_t game = e.rd32(q + 4), key = e.rd32(data + 8);
        uint32_t u = e.rd32(game + (key & 0x7f) * 4 + 0x1520);
        while (u && e.rd32(u + 0xc) != key) u = e.rd32(u + 0xe4);
        if (!u) return NE::lo(game, 0);                 // the vectors keep the anchor present: this exit answers the game pointer
        const uint32_t y = e.h(0x1130, 0, u), x = e.h(0x1100, 0, u), room = e.imp(0xA478, u);
        const uint32_t r = e.h(0xBA850, 0, 0, 0, room, 0, e.rd32(q + 4), x, data); e.edi(0xBA850, y);
        if (!r) return 0;
        e.wr8(data + 2, 0); e.wr8(data + 1, 0);
        return NE::lo(r, 1);
    }
    if (!e.rd8(data + 3)) return NE::lo(eax0, 1);
    if (e.rd8(data + 4)) {
        const uint32_t t = e.h(0xF8D10);                // the import GetTickCount through its slot
        if (e.rd32(data + 0x18) + 0x17318 < t) {
            e.wr8(data + 1, 0); e.wr8(data + 3, 0); e.wr8(data + 4, 0);
            const uint32_t game = e.rd32(q + 4);
            if (e.rd32(game + 0x70)) return NE::lo(game, 1);   // not taken by the vectors: the game pointer again
            return NE::lo(e.h(0x2E160, 0, 0, 0, 0, p0, 0), 1);
        }
    }
    uint32_t t = e.h(0xF8D10);
    if (e.rd32(data + 0x18) + 0x15f90 < t) {
        uint32_t r = t;
        if (e.rd8(data + 5)) {
            r = e.rd32(e.rd32(q + 4) + 0x70);
            if (!r) r = e.cb(p0, 0xBBCE0);
            e.wr8(data + 5, 0);
        }
        return NE::lo(r, 0);
    }
    t = e.h(0xF8D10);
    if (e.rd32(data + 0x18) + 0x124f8 < t && !e.rd8(data + 0x15)) {
        e.wr8(data + 0x15, 1);
        return NE::lo(e.h(0x2DE30, p0), 0);
    }
    return NE::lo(t, 0);
}
}
