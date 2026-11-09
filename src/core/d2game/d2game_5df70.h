// 1.13c D2Game+0x5DF70 ACT5Q6_PerEventCheck: ECX = p (game at +0, unit at +8); returns 0. Finds the type-0x24 quest node in the
// game's quest list (+0x10F4 cell), asks 10331(unit) for a target and 10826(target) for its mode; mode 0x83 with state byte +0x86 == 1
// runs 0x9E170 (EAX unit, EDI 0x84, stack game, 0xB), mode 0x83 otherwise does nothing, any other mode runs 0x9E170 (EDI 0x83, stack game, 0).
#pragma once
#include <cstdint>

namespace d2g5df70 {
template <class E> uint32_t Run(E& e, uint32_t p)
{
    const uint32_t game = e.rd32(p), lst = e.rd32(game + 0x10F4);
    if (!lst) return 0;
    uint32_t n = e.rd32(lst);
    while (n && e.rd32(n) != 0x24) n = e.rd32(n + 0xF4);
    if (!n) return 0;
    const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(p + 8));
    if (!a) return 0;
    const uint32_t m = e.h(0xA556, 0, 0, 0, 0, 0, a);
    if (m == 0x83) {
        if (e.rd8(e.rd32(n + 0x18) + 0x86) != 1) return 0;
        const uint32_t u = e.rd32(p + 8);
        e.h(0x9E170, u, 0, 0, 0, 0, game, 0xB); e.edi(0x9E170, 0x84);
        return 0;
    }
    const uint32_t u = e.rd32(p + 8);
    e.h(0x9E170, u, 0, 0, 0, 0, game, 0); e.edi(0x9E170, 0x83);
    return 0;
}
}
