// 1.13c D2Game+0x5EE30 ThreeSlotCounterTally: EBX = quest (game +4, state st +0x18), EDI = obj; void. The obj id (+0xC, -1 for no obj) must
// match one of st+0xEC+4i (i 0..2), else return. Mode obj+0x10 (0 for no obj): 1 runs 0xA4A2(obj, 2); 3 runs 0xA4A2(obj, 4); 2 raises
// st+0xFC+4i when byte st+0x108+i == 1, sets byte st+0x10B+i once the count passes 5 (signed), and runs 0xA4A2(obj, 3) when that byte is set.
// A matched slot always ends with 0x8EA80 (ECX game, EDX obj, stack 7, game+0xA8 + 0x19, 0,0,0).
#pragma once
#include <cstdint>

namespace d2g5ee30 {
template <class E> void Run(E& e, uint32_t q, uint32_t obj)
{
    const uint32_t id = obj ? e.rd32(obj + 0xC) : 0xFFFFFFFFu, st = e.rd32(q + 0x18);
    uint32_t i = 0;
    while (i < 3 && e.rd32(st + 0xEC + 4 * i) != id) i++;
    if (i == 3) return;
    const uint32_t mode = obj ? e.rd32(obj + 0x10) : 0;
    if (mode == 1) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    else if (mode == 3) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 4);
    else if (mode == 2) {
        if (e.rd8(st + 0x108 + i) == 1) e.wr32(st + 0xFC + 4 * i, e.rd32(st + 0xFC + 4 * i) + 1);
        if ((int32_t)e.rd32(st + 0xFC + 4 * i) > 5) e.wr8(st + 0x10B + i, 1);
        if (e.rd8(st + 0x10B + i)) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 3);
    }
    const uint32_t g = e.rd32(q + 4);
    e.h(0x8EA80, 0, g, obj, 0, 0, 7, e.rd32(g + 0xA8) + 0x19, 0, 0, 0);
}
}
