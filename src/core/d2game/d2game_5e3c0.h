// 1.13c D2Game+0x5E3C0 ACT5Q5_ResetOnClass78: EAX = unit, ESI = game; void. 10826(10331(unit)) must be 0x78 (no null test between them);
// the type-0x23 quest node (game+0x10F4 list) state st (+0x18) with byte st+0 == 0 gets st+0xC incremented, bytes st+0x11 and st+0x10
// cleared, and 0x5E0E0(game, node, st) runs. A missing node makes retail read address 0x18 (never framed).
#pragma once
#include <cstdint>

namespace d2g5e3c0 {
template <class E> void Run(E& e, uint32_t unit, uint32_t game)
{
    const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (e.h(0xA556, 0, 0, 0, 0, 0, a) != 0x78) return;
    const uint32_t lst = e.rd32(game + 0x10F4);
    uint32_t n = lst ? e.rd32(lst) : 0;
    while (n && e.rd32(n) != 0x23) n = e.rd32(n + 0xF4);
    if (!n) return;   // retail: reads [0x18]
    const uint32_t st = e.rd32(n + 0x18);
    if (e.rd8(st)) return;
    e.wr32(st + 0xC, e.rd32(st + 0xC) + 1);
    e.wr8(e.rd32(n + 0x18) + 0x11, 0);
    e.wr8(e.rd32(n + 0x18) + 0x10, 0);
    e.h(0x5E0E0, 0, 0, 0, 0, 0, game, n, e.rd32(n + 0x18));
}
}
