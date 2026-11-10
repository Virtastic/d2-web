// 1.13c D2Game+0xD62E0 (MoveItemInventoryToCursor): ECX = unit (self), EAX = inventory override (0 = self[0x60]),
// stack (ret 0x18) = game, item id, out word*, a3 (act), a4 (flag), a5 (skip the A6A6 gate). Returns 1 (item moved) or 0.
// Rule template over a call environment E (rd32/wr32, h(rva, eax, ecx, edx, ebx, esi, s0..s3), ed/edi). Fatal paths
// (Fog log + _exit) return 0: game or self null (0x907/0x908), no inventory (0x911), A748 mismatch (0x948/0x949).
// Retail keeps the item kind in the low byte of the out-pointer ARGUMENT SLOT and pushes the whole slot to the callees,
// so the words handed on are (out & ~0xFF) | kind: `slot` below. 1.13c delta vs D2MOO 1.10f: the 0xA5E6 trace call
// (line 0x966) and the 0x979 flag test are new; the 1.10f function had no kind byte.
#pragma once
#include <cstdint>

namespace rd62e0 {
template <class E> uint32_t Move(E& e, uint32_t self, uint32_t eaxInv, uint32_t game, uint32_t id, uint32_t out,
                                 uint32_t a3, uint32_t a4, uint32_t a5)
{
    if (!game || !self) return 0;
    e.wr32(out, 0);
    uint32_t inv = e.rd32(self + 0x60);
    if (eaxInv) inv = eaxInv;
    if (!inv) return 0;
    if (e.h(0xA6A6, 0, 0, 0, 0, 0, inv) && !a5) return 0;
    uint32_t it = e.rd32(game + 0x1720 + (id & 0x7F) * 4);   // the unit hash: link +0xE4, id +0xC
    while (it && e.rd32(it + 0xC) != id) it = e.rd32(it + 0xE4);
    if (!it) { e.wr32(out, 1); return 0; }
    if (e.rd32(it) != 4) return 0;
    if (e.rd32(self) == 0 && !e.h(0x37680, 0, 0, 0, 0, self) && (e.h(0xA6C4, 0, 0, 0, 0, 0, it) & 0xFF)) { e.wr32(out, 1); return 0; }
    if (e.rd32(it + 0x10)) { e.wr32(out, 1); return 0; }
    e.h(0xD29F0, 0, 0, 0, 0, 0, self);
    e.wr32(it + 0xC4, e.rd32(it + 0xC4) & ~2u);
    const uint32_t p = e.h(0xA748, 0, 0, 0, 0, 0, inv, it);
    if (!p || p != it) return 0;
    e.ed(it); e.h(0xD46B0, 0, 0, 0, 0, self, game, 0, 1); e.edi(0xD46B0, it);
    if (e.h(0xA640, 0, 0, 0, 0, 0, it, self)) { e.h(0xD51A0, 0, 0, 0, 0, 0, game, self, 0); e.h(0xA9A6, 0, 0, 0, 0, 0, self, 1); }
    e.h(0xD4630, it, self);
    const uint32_t kind = e.h(0xA6C4, 0, 0, 0, 0, 0, it) & 0xFF;
    const uint32_t slot = (out & ~0xFFu) | kind;
    const uint32_t r = e.h(0xA634, 0, 0, 0, 0, 0, self, slot, e.rd32(game + 0x70));
    uint32_t ebx = 1, a5p = 0;
    if (e.rd32(self) != 0 || kind == 1) ebx = 0; else if (kind == 2) a5p = 1;
    const uint32_t A = e.h(0x1130, 0, it, 0, 0, 0, r, 0, slot);
    const uint32_t B = e.h(0x1100, 0, it, 0, 0, 0, A);
    e.h(0xA5E6, 0, 0, 0, 0, 0, 0xB20FB14Cu, 0x966, slot, B);   // hooked trace: file, line, slot, B (see the descriptor)
    e.h(0xA958, 0, 0, 0, 0, 0, it, slot);
    e.h(0xA92E, 0, 0, 0, 0, 0, it, 0xFF);
    if (ebx) e.h(0xAA54, 0, 0, 0, 0, 0, inv, it);
    e.h(0xA4A2, 0, 0, 0, 0, 0, it, 4);
    if (a3 != 1) return 1;
    if (a4) e.h(0xA65E, 0, 0, 0, 0, 0, it, 0x20, 1);
    if (a5p == 1) e.h(0x71330, 0, game, self, 0, 0, it);
    e.h(0xA916, 0, 0, 0, 0, 0, it, 4, 1);
    if (e.h(0xD1BC0, it)) e.h(0xA65E, 0, 0, 0, 0, 0, it, 1, 1);
    if (e.h(0xA8A4, 0, 0, 0, 0, 0, it, 0x4000, 0x979, 0xB20FB14Cu)) e.h(0xA65E, 0, 0, 0, 0, 0, it, 0x4000, 0);
    e.h(0xA994, 0, 0, 0, 0, 0, inv, it);
    e.h(0xA9A6, 0, 0, 0, 0, 0, self, 1);
    return 1;
}
}
