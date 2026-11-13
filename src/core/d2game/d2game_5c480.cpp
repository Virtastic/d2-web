// 1.13c D2Game+0x5C480 -- two-step quest timer: wakes the stored monster, then replaces it with the 0x20F monster and arms the next timer
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
static uint32_t unitById(NE& e, uint32_t game, uint32_t id) {   // the game's unit table of this kind: 128 chains at +0x1520, id +0xC, next +0xE4
    uint32_t u = e.rd32(game + 0x1520 + 4 * (id & 0x7f));
    while (u && e.rd32(u + 0xc) != id) u = e.rd32(u + 0xe4);
    return u;
}
uint32_t f5c480(NE& e, uint32_t eax0, uint32_t game, uint32_t q)
{
    const uint32_t data = e.rd32(q + 0x18);
    uint32_t eax = NE::lo(eax0, e.rd8(data + 0x90));
    if (e.rd8(data + 0x90)) return NE::lo(eax, 0);
    const uint32_t u = unitById(e, game, e.rd32(data + 0xfc));
    eax = e.rd32(data + 0x8c);
    if (!eax) {
        if (u) { e.imp(0xA4A2, u, 2); eax = e.imp(0xA550, u); }
        e.wr32(data + 0x8c, 1);
        return NE::lo(eax, 0);
    }
    if (eax != 1) return NE::lo(eax, 0);
    e.wr32(data + 0x8c, 2);
    if (!u) return NE::lo(eax, 0);
    const uint32_t o = e.h(0x361E0, 0, 0, 0, 0, 0, game, e.rd32(data + 0xf0), e.rd32(data + 0xf4), e.imp(0xA478, u), 1, 0x20f);
    if (!o) return 0;
    e.imp(0xA44E, e.imp(0xA478, u), e.rd32(u), e.rd32(u + 0xc));
    e.imp(0xA63A, u);
    const uint32_t p = e.rd32(data + 0x104);
    if (p && e.rd32(p + 4) && !e.rd8(data + 0x10c)) {
        const uint32_t md = o && e.rd32(o) == 1 ? e.rd32(o + 0x14) : 0;   // 0x20DE0: the monster record's +0x28 block, +0x38
        e.h(0xA562, 0, data + 0x104, md ? e.rd32(md + 0x28) + 0x38 : 0x38);
        e.wr8(data + 0x10c, 1);
    }
    e.wr32(data + 0x98, e.rd32(o + 0xc));
    e.wr8(data + 0x90, 1);
    eax = e.h(0x56B10, 0, q);
    e.wr8(data + 0x100, 0);
    return NE::lo(eax, 1);
}
}
