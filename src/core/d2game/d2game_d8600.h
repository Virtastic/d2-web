// 1.13c D2Game+0xD8600 (move an item into a belt/body slot, the occupant out): ECX = unit (self), EAX = item id, stack (ret 0x10) = game,
// a2 (slot byte, >= 11 returns 0 before anything is written), a3 (flag), out word*. Returns 1 (moved) or 0. AA78 must return 5 and the
// occupant must have mode 1. Rule template over a call environment E as d2game_d8020.h. Fatal paths return 0: game or self null
// (0xCA8/0xCA9), A748 null or not the occupant (0xCEF/0xCF0). Unlike D8020, the not-found and mode-mismatch exits leave out := 0.
#pragma once
#include <cstdint>

namespace rd8600 {
template <class E> uint32_t Move(E& e, uint32_t self, uint32_t id, uint32_t game, uint32_t a2, uint32_t a3, uint32_t out)
{
    if (!game || !self) return 0;
    if ((a2 & 0xFF) >= 0xB) return 0;
    e.wr32(out, 0);
    e.h(0xD29F0, 0, 0, 0, 0, 0, self);
    uint32_t it = e.rd32(game + 0x1720 + (id & 0x7F) * 4);
    while (it && e.rd32(it + 0xC) != id) it = e.rd32(it + 0xE4);
    if (!it) { e.wr32(out, 0); return 0; }
    if (e.rd32(it) != 4) return 0;
    if (e.rd32(it + 0x10) != 4) { e.wr32(out, 0); return 0; }
    const uint32_t slot = a2 & 0xFF;
    if (e.h(0xAA78, 0, 0, 0, 0, 0, self, slot, it, a3) != 5) { e.wr32(out, 0); return 0; }
    const uint32_t inv = e.rd32(self + 0x60);
    const uint32_t t = e.h(0xA54A, 0, 0, 0, 0, 0, inv);
    if (t) e.h(0xA9BE, 0, 0, 0, 0, 0, self, t);
    uint32_t other = e.h(0xA7AE, 0, 0, 0, 0, 0, inv, slot);
    e.h(0xA850, 0, 0, 0, 0, 0, self, inv, e.stk(&other), slot);
    if (!other || e.rd32(other + 0x10) != 1) { e.wr32(out, 1); return 0; }
    e.ed(other); e.h(0xD46B0, 0, 0, 0, 0, self, game, 0, 1); e.edi(0xD46B0, other);
    if (a3 == 0 && !e.h(0xA934, 0, 0, 0, 0, 0, it, self, 0, 0, 0, 0)) {
        e.ed(other); e.h(0xD47C0, 0, 0, 0, inv, self, 0); e.edi(0xD47C0, other);
        e.h(0xDFFD0, 0, 0, 0, 0, self, 0x14, 0);
        return 0;
    }
    if (e.h(0xAA66, 0, 0, 0, 0, 0, it) == 0x13) e.h(0xD7E30, it, 0, 0, 0, 0, game, self);
    e.h(0xA8AA, 0, 0, 0, 0, 0, self, other);
    e.h(0xA946, 0, 0, 0, 0, 0, inv, other);
    const uint32_t p = e.h(0xA748, 0, 0, 0, 0, 0, inv, other);
    if (!p || p != other) return 0;
    e.h(0xA742, 0, 0, 0, 0, 0, inv, slot);
    e.h(0xAA54, 0, 0, 0, 0, 0, inv, other);
    if (other) e.wr32(other + 0xC4, e.rd32(other + 0xC4) & ~2u);
    e.h(0xA4A2, 0, 0, 0, 0, 0, other, 4);
    e.h(0xA65E, 0, 0, 0, 0, 0, other, 0x80, 1);
    e.h(0xA916, 0, 0, 0, 0, 0, other, 0x20, 1);
    e.h(0xA65E, 0, 0, 0, 0, 0, other, 1, 1);
    if (e.h(0xA8A4, 0, 0, 0, 0, 0, other, 0x4000, 0xCF8, 0xB20FB14Cu)) e.h(0xA65E, 0, 0, 0, 0, 0, other, 0x4000, 0);
    e.h(0xA994, 0, 0, 0, 0, 0, inv, other);
    if (!e.h(0xA9A0, 0, 0, 0, 0, 0, inv, it, slot)) { e.wr32(out, 1); return 0; }
    if (!e.h(0xA9B8, 0, 0, 0, 0, 0, inv, it, 3)) { e.wr32(out, 1); return 0; }
    e.h(0xA86E, 0, 0, 0, 0, 0, it, a2);
    e.h(0xA910, 0, 0, 0, 0, 0, inv, it);
    e.ed(it); e.h(0xD47C0, 0, 0, 0, inv, self, 0); e.edi(0xD47C0, it);
    e.wr32(it + 0xC4, e.rd32(it + 0xC4) & ~2u);
    e.h(0xA4A2, 0, 0, 0, 0, 0, it, 1);
    e.h(0xA92E, 0, 0, 0, 0, 0, it, 0xFF);
    e.h(0xA65E, 0, 0, 0, 0, 0, it, 0x40, 1);
    e.h(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
    e.h(0xA916, 0, 0, 0, 0, 0, it, 0x20, 1);
    e.h(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
    if (e.h(0xA8A4, 0, 0, 0, 0, 0, it, 0x4000, 0xD13, 0xB20FB14Cu)) e.h(0xA65E, 0, 0, 0, 0, 0, it, 0x4000, 0);
    e.h(0xA994, 0, 0, 0, 0, 0, inv, it);
    e.h(0xD28D0, self);
    e.h(0xD51A0, 0, 0, 0, 0, 0, game, self, 0);
    return 1;
}
}
