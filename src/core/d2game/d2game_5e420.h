// 1.13c D2Game+0x5E420 ACT5Q5_OnSlotEvent: stdcall (q, obj), ret 8, void. st = q+0x18. Returns at once when obj+0x10 == 4, st+0x40 >= 3
// (signed) or obj == 0. obj+4 picks the monster row (0x1DA: word dt+0xB3A, slot 1; 0x1DB: dt+0xB36, slot 0; 0x1DC: dt+0xB38, slot 2, dt =
// *sgptDataTables); any other class returns. st+0xC > 0 runs 0x5E0E0(q+4, q, st). Else 10xxx thunk 0xA544(obj, &pos) fills a two-word
// position, 0x36820 (EAX row, ECX 2, ESI &pos, stack game, obj) spawns; no unit runs 0x8EA80 (ECX game, EDX obj, stack 7, game+0xA8 + 10, 0,0,0).
// A unit: obj+0x10 != 4 runs 0xA4A2(obj, 4) and sets st+0x30+4*slot = 4; then byte st+0x2C+k = 1 and st+0x20+4k = unit+0xC (k = st+0x40),
// st+0x40 and st+0x44 rise by one, byte st+0x48 = 1.
#pragma once
#include <cstdint>

namespace d2g5e420 {
template <class E> void Run(E& e, uint32_t q, uint32_t obj, uint32_t dtCell)
{
    const uint32_t st = e.rd32(q + 0x18);
    if (obj && e.rd32(obj + 0x10) == 4) return;
    if ((int32_t)e.rd32(st + 0x40) >= 3) return;
    if (!obj) return;   // class -1: no case
    uint32_t off, slot;
    switch (e.rd32(obj + 4)) {
    case 0x1DA: off = 0xB3A; slot = 1; break;
    case 0x1DB: off = 0xB36; slot = 0; break;
    case 0x1DC: off = 0xB38; slot = 2; break;
    default: return;
    }
    const uint32_t row = e.rd32(e.rd32(dtCell) + off) & 0xFFFF;
    if ((int32_t)e.rd32(st + 0xC) > 0) { e.h(0x5E0E0, 0, 0, 0, 0, 0, e.rd32(q + 4), q, st); return; }
    uint32_t pos[2] = { 0, 0 };
    e.h(0xA544, 0, 0, 0, 0, 0, obj, (uint32_t)(uintptr_t)pos);
    const uint32_t game = e.rd32(q + 4);
    const uint32_t u = e.h(0x36820, row, 2, 0, 0, (uint32_t)(uintptr_t)pos, game, obj);
    e.blk(0x36820, pos, 2);
    if (!u) {
        const uint32_t g = e.rd32(q + 4);
        e.h(0x8EA80, 0, g, obj, 0, 0, 7, e.rd32(g + 0xA8) + 0xA, 0, 0, 0);
        return;
    }
    if (e.rd32(obj + 0x10) != 4) { e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 4); e.wr32(st + 0x30 + slot * 4, 4); }
    e.wr8(st + 0x2C + e.rd32(st + 0x40), 1);
    e.wr32(st + 0x20 + e.rd32(st + 0x40) * 4, e.rd32(u + 0xC));
    e.wr32(st + 0x40, e.rd32(st + 0x40) + 1);
    e.wr32(st + 0x44, e.rd32(st + 0x44) + 1);
    e.wr8(st + 0x48, 1);
}
}
