// 1.13c D2Game+0x5E560 (sub_5E560): stdcall (q, st), ret 8, returns 0/1. The three object ids st+0x14+4i are looked up in q+4's object hash
// (+0x1520 + (id & 0x7F)*4, chain +0xE4, id +0xC); each hit runs 0xA51A(10331(obj), 0), the first miss returns 0. Then per object:
// 0xA4A2(obj, 3), 0xA550(obj), 0x8EA80 (ECX game, EDX obj, stack 7, game+0xA8 + 0x14, 0,0,0), st+0x30+4i = 3; st+0x40 = st+0x44 = 0, return 1.
// q+4 == 0 is retail's fatal-error exit (never framed).
#pragma once
#include <cstdint>
#include "d2game_5e0e0.h"

namespace d2g5e560 {
template <class E> uint32_t Run(E& e, uint32_t q, uint32_t st)
{
    uint32_t o[3];
    for (uint32_t i = 0; i < 3; i++) {
        const uint32_t g = e.rd32(q + 4);
        if (!g) return 0;   // retail: Fog error and exit
        o[i] = d2g5e0e0::Obj(e, g, e.rd32(st + 0x14 + 4 * i));
        if (!o[i]) return 0;
        const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, o[i]);
        e.h(0xA51A, 0, 0, 0, 0, 0, a, 0);
    }
    for (uint32_t i = 0; i < 3; i++) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, o[i], 3);
        e.h(0xA550, 0, 0, 0, 0, 0, o[i]);
        const uint32_t g = e.rd32(q + 4);
        e.h(0x8EA80, 0, g, o[i], 0, 0, 7, e.rd32(g + 0xA8) + 0x14, 0, 0, 0);
        e.wr32(st + 0x30 + 4 * i, 3);
    }
    e.wr32(st + 0x40, 0);
    e.wr32(st + 0x44, 0);
    return 1;
}
}
