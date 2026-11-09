// 1.13c D2Game+0x5E660 (sub_5E660): ECX = game, EDX = q; returns AL. st = q+0x18. st+0xC != 0 or byte st+0x11 == 0: byte st+4 = 0, return 1.
// Else per slot with byte st+0x3C+i set: the object id st+0x14+4i is looked up in q+4's object hash; a hit runs 0xA4A2(obj, 1),
// 0xA520(obj+4) (row), 0x8EA80 (ECX game, EDX obj, stack 1, (row+0xDC >> 8) + game+0xA8, 0,0,0), clears the byte and sets st+0x30+4i = 2;
// a miss marks the call failed. Failed: return 0 (st+4 untouched); else byte st+4 = 0, return 1. q+4 == 0 is the fatal-error exit.
#pragma once
#include <cstdint>
#include "d2game_5e0e0.h"

namespace d2g5e660 {
template <class E> uint32_t Run(E& e, uint32_t game, uint32_t q)
{
    const uint32_t st = e.rd32(q + 0x18);
    if (e.rd32(st + 0xC) || !e.rd8(st + 0x11)) { e.wr8(st + 4, 0); return 1; }
    bool miss = false;
    for (uint32_t i = 0; i < 3; i++) {
        if (!e.rd8(st + 0x3C + i)) continue;
        const uint32_t g = e.rd32(q + 4);
        if (!g) return 0;   // retail: Fog error and exit
        const uint32_t o = d2g5e0e0::Obj(e, g, e.rd32(st + 0x14 + 4 * i));
        if (!o) { miss = true; continue; }
        e.h(0xA4A2, 0, 0, 0, 0, 0, o, 1);
        const uint32_t row = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(o + 4));
        e.h(0x8EA80, 0, game, o, 0, 0, 1, (e.rd32(row + 0xDC) >> 8) + e.rd32(game + 0xA8), 0, 0, 0);
        e.wr8(st + 0x3C + i, 0);
        e.wr32(st + 0x30 + 4 * i, 2);
    }
    if (miss) return 0;
    e.wr8(st + 4, 0);
    return 1;
}
}
