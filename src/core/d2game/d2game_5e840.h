// 1.13c D2Game+0x5E840 (sub_5E840): EBX = quest (game +4, state st +0x18), EDI = obj; void. Mode obj+0x10 (0 for no obj): 1 runs
// 0xA4A2(obj, 2); 3 runs 0xA4A2(obj, 4); 2 asks 10826(10331(obj)): 0x6D raises st+0xDC and sets byte st+0xAE once it passes 5 (signed),
// then byte st+0xAE set runs 0xA4A2(obj, 3); any other answer runs 0xA4A2(obj, 3) when byte st+0xAD is set. Every path ends with 0x8EA80
// (ECX game, EDX obj, stack 7, game+0xA8 + 0x19, 0,0,0).
#pragma once
#include <cstdint>

namespace d2g5e840 {
template <class E> void Run(E& e, uint32_t q, uint32_t obj)
{
    const uint32_t mode = obj ? e.rd32(obj + 0x10) : 0;
    if (mode == 1) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    else if (mode == 3) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 4);
    else if (mode == 2) {
        const uint32_t st = e.rd32(q + 0x18);
        const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, obj);
        if (e.h(0xA556, 0, 0, 0, 0, 0, a) == 0x6D) {
            const uint32_t c = e.rd32(st + 0xDC) + 1;
            e.wr32(st + 0xDC, c);
            if ((int32_t)c > 5) e.wr8(st + 0xAE, 1);
            if (e.rd8(st + 0xAE)) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 3);
        } else if (e.rd8(st + 0xAD)) e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 3);
    }
    const uint32_t g = e.rd32(q + 4);
    e.h(0x8EA80, 0, g, obj, 0, 0, 7, e.rd32(g + 0xA8) + 0x19, 0, 0, 0);
}
}
