// 1.13c D2Game+0xD8020 (swap the item in body slot 4/5 with the one in the other): ECX = unit (self), EAX = item id, stack (ret 0x10) =
// game, a2 (body location byte: 5 maps to slot 4, 4 to slot 5, else out := 1), a3 (flag), out word*. Returns 1 (moved) or 0.
// Rule template over a call environment E (rd32/wr32, h(rva, eax, ecx, edx, ebx, esi, s0..s3), ed/edi, stk). Fatal paths (Fog log + _exit)
// return 0: game or self null (0xB7C/0xB7D), A748 null or not the occupant (0xBC9/0xBCA). Retail writes the occupant back through A850's
// out-pointer; hooked, the local keeps the A7AE value.
#pragma once
#include <cstdint>

namespace rd8020 {
template <class E> uint32_t Swap(E& e, uint32_t self, uint32_t id, uint32_t game, uint32_t a2, uint32_t a3, uint32_t out)
{
    if (!game || !self) return 0;
    e.wr32(out, 0);
    e.h(0xD29F0, 0, 0, 0, 0, 0, self);
    uint32_t it = e.rd32(game + 0x1720 + (id & 0x7F) * 4);   // the unit hash: link +0xE4, id +0xC
    while (it && e.rd32(it + 0xC) != id) it = e.rd32(it + 0xE4);
    if (!it) { e.wr32(out, 1); return 0; }
    if (e.rd32(it) != 4) return 0;
    if (e.rd32(it + 0x10) != 4) { e.wr32(out, 1); return 0; }
    const uint32_t loc = a2 & 0xFF;
    uint32_t slot;
    if (loc == 5) slot = 4; else if (loc == 4) slot = 5; else { e.wr32(out, 1); return 0; }
    const uint32_t inv = e.rd32(self + 0x60);
    uint32_t other = e.h(0xA7AE, 0, 0, 0, 0, 0, inv, slot);
    if (!other) { e.wr32(out, 1); return 0; }
    e.h(0xA850, 0, 0, 0, 0, 0, self, inv, e.stk(&other), slot);
    if (!other) { e.wr32(out, 1); return 0; }
    if (e.h(0xAA78, 0, 0, 0, 0, 0, self, loc, it, a3) != 2) { e.wr32(out, 1); return 0; }
    e.ed(other); e.h(0xD46B0, 0, 0, 0, 0, self, game, 0, 1); e.edi(0xD46B0, other);
    if (a3 == 0 && !e.h(0xA934, 0, 0, 0, 0, 0, it, self, 0, 0, 0, 0)) {
        e.ed(other); e.h(0xD47C0, 0, 0, 0, inv, self, 0); e.edi(0xD47C0, other);
        e.h(0xDFFD0, 0, 0, 0, 0, self, 0x14, 0);
        return 0;
    }
    const uint32_t t = e.h(0xA54A, 0, 0, 0, 0, 0, inv);
    if (t) e.h(0xA9BE, 0, 0, 0, 0, 0, self, t);
    e.h(0xA8AA, 0, 0, 0, 0, 0, self, other);
    e.h(0xA946, 0, 0, 0, 0, 0, inv, other);
    const uint32_t p = e.h(0xA748, 0, 0, 0, 0, 0, inv, other);
    if (!p || p != other) return 0;
    e.h(0xA742, 0, 0, 0, 0, 0, inv, slot);
    if (e.h(0xAA66, 0, 0, 0, 0, 0, p) == 0x13) e.h(0xD7E30, 0, 0, 0, 0, 0, game, self);
    e.h(0xAA54, 0, 0, 0, 0, 0, inv, other);
    if (other) e.wr32(other + 0xC4, e.rd32(other + 0xC4) & ~2u);
    e.h(0xA4A2, 0, 0, 0, 0, 0, other, 4);
    if (e.h(0xA9A0, 0, 0, 0, 0, 0, inv, it, loc)) {
        if (!e.h(0xA9B8, 0, 0, 0, 0, 0, inv, it, 3)) { e.wr32(out, 1); return 0; }
        e.h(0xA86E, 0, 0, 0, 0, 0, it, a2);
        e.h(0xA910, 0, 0, 0, 0, 0, inv, it);
        e.ed(it); e.h(0xD47C0, 0, 0, 0, inv, self, 0); e.edi(0xD47C0, it);
        e.wr32(it + 0xC4, e.rd32(it + 0xC4) & ~2u);
        e.h(0xA4A2, 0, 0, 0, 0, 0, it, 1);
        e.h(0xA92E, 0, 0, 0, 0, 0, it, 0xFF);
        e.h(0xA916, 0, 0, 0, 0, 0, it, 0x10000, 1);
        e.h(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
        if (e.h(0xA8A4, 0, 0, 0, 0, 0, it, 0x4000, 0xBE7, 0xB20FB14Cu)) e.h(0xA65E, 0, 0, 0, 0, 0, it, 0x4000, 0);
        e.h(0xA994, 0, 0, 0, 0, 0, inv, it);
        e.h(0xD28D0, self);
        e.h(0xD51A0, 0, 0, 0, 0, 0, game, self, 0);
    }
    e.h(0xA9A6, 0, 0, 0, 0, 0, self, 1);
    return 1;
}
}
