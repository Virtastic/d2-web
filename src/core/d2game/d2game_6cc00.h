// 1.13c D2Game+0x6CC00 (UNIT_BindType3BById): EAX = unit id, EBX = state,
// [esp+4] = game, [esp+8] unused (ret 8). Rule template over a call environment E (rd32/wr8 and h(rva, regs..., stack...)).
// Retail keeps its flag byte in the low byte of the game ARGUMENT SLOT and pushes the whole slot to #10111, so the callee
// receives (game & ~0xFF) | flag; `slot` carries that word (it starts as the game argument) and both writes go through it.
#pragma once
#include <cstdint>

namespace r6cc00 {
template <class E> void Bind(E& e, uint32_t id, uint32_t ebx, uint32_t game, uint32_t& slot)
{
    if (!game) return;   // retail: Fog log 0x11DD, _exit(-1)
    uint32_t u = e.rd32(game + 0x1520 + (id & 0x7F) * 4);   // the unit hash: 128 buckets, link +0xE4, id +0xC
    while (u && e.rd32(u + 0xC) != id) u = e.rd32(u + 0xE4);
    if (!u || e.rd32(u + 4) != 0x3B) return;
    if (e.h(0xB17A, 0, 0, 0, 0, 0, u, 1)) return;                       // #10216
    const uint32_t p = e.h(0xE1570, 0, 0, 0, 0, 0, game, u);            // the partner
    if (!p) return;
    if (e.rd32(p + 4) != 0x3B) return;                                  // retail: log 0x11EA, _exit(-1)
    e.wr8(e.rd32(u + 0x14) + 4, ebx);                                   // bl > 0xFF (log 0x11EC) cannot happen
    slot = (slot & ~0xFFu) | ((e.h(0xAEE0, 0, 0, 0, 0, 0, u) | 1) & 0xFF);   // #10258, or 1
    e.h(0xAEC8, 0, 0, 0, 0, 0, u, slot);                                 // #10111
    e.h(0xA4A2, 0, 0, 0, 0, 0, u, 3);                                    // #11090: mode 3
    e.h(0xA44E, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, p), 2, e.rd32(p + 0xC));   // #10885(#10331(partner), 2, partner id)
    e.h(0xE20A0, 0, game, p);
    const uint32_t x = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, u));   // #10826(#10331(unit))
    const uint32_t nu = e.h(0xA22E0, 0, 0, 0, 0, 0, game, u, ebx, x);
    if (!nu) return;                                                     // retail: log 0x11FB, _exit(-1)
    slot = (slot & ~0xFFu) | ((e.h(0xAEE0, 0, 0, 0, 0, 0, nu) | 3) & 0xFF);
    e.h(0xAEC8, 0, 0, 0, 0, 0, nu, slot);
}
}
