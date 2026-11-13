// 1.13c D2Game+0x5AB60 -- message handler: the join message and the 0x78 step, then wakes the quest monster
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
static uint32_t unitById(NE& e, uint32_t game, uint32_t id) {   // the game's unit table of this kind: 128 chains at +0x1520, id +0xC, next +0xE4
    uint32_t u = e.rd32(game + 0x1520 + 4 * (id & 0x7f));
    while (u && e.rd32(u + 0xc) != id) u = e.rd32(u + 0xe4);
    return u;
}
uint32_t f5ab60(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    uint32_t eax = eax0;
    if (e.rd32(a + 0x14) == 0x6d) {
        if (NE::rd16(q + 0x9c)) { const uint32_t pl = e.rd32(a + 0xc); eax = e.h(0x33830, pl ? e.rd32(pl + 0xc) : 0xffffffffu, q + 0x1c); }
        eax = NE::lo(eax, e.rd8(q + 9));
        if (!e.rd8(q + 9)) return eax;
        if (e.rd8(q + 0xc) == 2) {
            eax = e.fl(e.qbuf(e.rd32(a), e.rd32(a + 0xc)), e.rd32(q + 0xe0), 0);
            if (eax == 1) return eax;
            e.h(0x35EB0, 3);
            if (e.rd8(q + 0xb) != 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1); e.cb(game, 0x58F90); }
            eax = e.cb(game, 0x57750);
        }
    }
    if (e.rd32(a + 0x18) != 0x78) return eax;
    bool did = false;
    eax = NE::lo(eax, 0);
    if (e.rd8(q + 0xb) < 2 && e.rd8(q + 0xc) > 2) { e.wr8(q + 0xb, 2); e.wr8(q + 0x14, 0); eax = NE::lo(e.cb(game, 0x58F90), 1); did = true; }
    if (e.rd8(q + 0xc) < 3) { e.wr8(q + 0xc, 3); e.say(game, 3); did = true; }
    if (did) eax = e.cb(game, 0x57750);
    if (e.rd8(data)) return eax;
    eax = NE::lo(eax, e.rd8(data + 0x58));
    if (!e.rd8(data + 0x58)) return eax;
    const uint32_t u = unitById(e, game, e.rd32(data + 0x5c));
    if (!u || e.rd32(data + 0x60)) return u;
    e.wr32(data + 0x60, 2);
    return e.imp(0xA4A2, u, 1);
}
}
