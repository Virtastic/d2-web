// 1.13c D2Game+0x5EC30 (sub_5EC30): stdcall (q), ret 4, void. st = q+0x18, g = q+4 (g == 0 is retail's fatal-error exit, never framed).
// A: byte st+0x91 set and st+0x84 != 2: the unit id st+0x94 is looked up in g's type-1 hash (+0x1320). Miss: 0x1EC40 (AL 4 over the id,
// ECX 0x200, EBX g). Hit: 0x48370 (EAX = type-1 unit's pdata+0x30, else 0) set runs 0x48410(g, u, 0); then 11103(u, 0), 0x75E10(g, block
// {0xC, u, 0,0,0, 0x6400, 0,0}, 1), 10885(10331(u), u+0, id), 10408(u), 11016(u). Byte st+0x91 = 0 either way.
// B: byte st+0x93 set: the unit id st+0x9C is looked up the same way. Miss: 0x1EC40 (ECX 0x202). Hit: 0x5DD00 (ESI u, stack q), then the
// object id st+0x110 in g's object hash (+0x1520); an object runs 0x8EA80 (ECX g, EDX obj, stack 7, g+0xA8 + 1, 0,0,0). Byte st+0x93 = 0.
// C: st+0x88 == 0 becomes 1.
#pragma once
#include <cstdint>
#include "d2game_5dd00.h"
#include "d2game_5e0e0.h"

namespace d2g5ec30 {
template <class E> uint32_t Unit(E& e, uint32_t g, uint32_t id)
{
    uint32_t u = e.rd32(g + 0x1320 + (id & 0x7F) * 4);
    while (u && e.rd32(u + 0xC) != id) u = e.rd32(u + 0xE4);
    return u;
}
template <class E> void Run(E& e, uint32_t q)
{
    const uint32_t st = e.rd32(q + 0x18);
    if (e.rd8(st + 0x91) && e.rd32(st + 0x84) != 2) {
        const uint32_t g = e.rd32(q + 4);
        if (!g) return;   // retail: Fog error and exit
        const uint32_t id = e.rd32(st + 0x94), u = Unit(e, g, id);
        if (!u) e.h(0x1EC40, (id & 0xFFFFFF00u) | 4, 0x200, 0, g);
        else {
            uint32_t pd = 0;
            if (e.rd32(u) == 1 && e.rd32(u + 0x14)) pd = e.rd32(e.rd32(u + 0x14) + 0x30);
            if (e.h(0x48370, pd)) e.h(0x48410, 0, 0, 0, 0, 0, g, u, 0);
            e.h(0xA496, 0, 0, 0, 0, 0, u, 0);
            d2g5dd00::Block(e, e.rd32(q + 4), u);
            const uint32_t id2 = e.rd32(st + 0x94), t = e.rd32(u);
            const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, u);
            e.h(0xA44E, 0, 0, 0, 0, 0, a, t, id2);
            e.h(0xA550, 0, 0, 0, 0, 0, u);
            e.h(0xA63A, 0, 0, 0, 0, 0, u);
        }
        e.wr8(st + 0x91, 0);
    }
    if (e.rd8(st + 0x93)) {
        const uint32_t g = e.rd32(q + 4);
        if (!g) return;   // retail: Fog error and exit
        const uint32_t id = e.rd32(st + 0x9C), u = Unit(e, g, id);
        if (!u) e.h(0x1EC40, (id & 0xFFFFFF00u) | 4, 0x202, 0, g);
        else {
            e.h(0x5DD00, 0, 0, 0, 0, u, q);
            const uint32_t g2 = e.rd32(q + 4);
            if (!g2) return;   // retail: Fog error and exit
            const uint32_t o = d2g5e0e0::Obj(e, g2, e.rd32(st + 0x110));
            if (o) {
                const uint32_t g3 = e.rd32(q + 4);
                e.h(0x8EA80, 0, g3, o, 0, 0, 7, e.rd32(g3 + 0xA8) + 1, 0, 0, 0);
            }
        }
        e.wr8(st + 0x93, 0);
    }
    if (!e.rd32(st + 0x88)) e.wr32(st + 0x88, 1);
}
}
