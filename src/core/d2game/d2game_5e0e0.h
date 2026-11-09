// 1.13c D2Game+0x5E0E0 ACT5Q5_ResetSlots: stdcall (game, node, st), ret 0xC, void. For each of the three slots with byte st+0x2C+i set,
// 0x5E000 (EBX game, EDI id st+0x20+4i) either releases it (st+0x40 and st+0x44 drop by one) or 0x1EC40 runs (AL 4, ECX 0x21C+i, EBX game);
// the byte is cleared. Then st+0x48 = 0; per slot st+0x3C+i = 0, the object id st+0x14+4i is looked up in node+4's object hash
// (+0x1520 + (id & 0x7F)*4, chain +0xE4, id +0xC), a hit runs 10xxx thunk 0xA4A2(obj, 0), and st+0x30+4i = 0. st+0x4C = 0; the object id
// st+0x50 is looked up in game's object hash, a hit runs 0xA4A2(obj, 0). node+4 == 0 or game == 0 is retail's fatal-error exit (never framed).
#pragma once
#include <cstdint>

namespace d2g5e0e0 {
template <class E> uint32_t Obj(E& e, uint32_t g, uint32_t id)
{
    uint32_t o = e.rd32(g + 0x1520 + (id & 0x7F) * 4);
    while (o && e.rd32(o + 0xC) != id) o = e.rd32(o + 0xE4);
    return o;
}
template <class E> void Run(E& e, uint32_t game, uint32_t node, uint32_t st)
{
    for (uint32_t i = 0; i < 3; i++) {
        if (!e.rd8(st + 0x2C + i)) continue;
        const uint32_t id = e.rd32(st + 0x20 + 4 * i);
        const uint32_t r = e.h(0x5E000, 0, 0, 0, game); e.edi(0x5E000, id);
        if (r & 0xFF) {
            e.wr32(st + 0x40, e.rd32(st + 0x40) - 1);
            e.wr32(st + 0x44, e.rd32(st + 0x44) - 1);
        } else {
            e.h(0x1EC40, (r & 0xFFFFFF00u) | 4, 0x21C + i, 0, game);
        }
        e.wr8(st + 0x2C + i, 0);
    }
    e.wr8(st + 0x48, 0);
    for (uint32_t i = 0; i < 3; i++) {
        e.wr8(st + 0x3C + i, 0);
        const uint32_t g = e.rd32(node + 4);
        if (!g) return;   // retail: Fog error and exit
        const uint32_t o = Obj(e, g, e.rd32(st + 0x14 + 4 * i));
        if (o) e.h(0xA4A2, 0, 0, 0, 0, 0, o, 0);
        e.wr32(st + 0x30 + 4 * i, 0);
    }
    e.wr32(st + 0x4C, 0);
    if (!game) return;   // retail: Fog error and exit
    const uint32_t o = Obj(e, game, e.rd32(st + 0x50));
    if (o) e.h(0xA4A2, 0, 0, 0, 0, 0, o, 0);
}
}
