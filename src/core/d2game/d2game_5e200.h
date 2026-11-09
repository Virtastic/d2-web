// 1.13c D2Game+0x5E200 ACT5Q5_TryResetOnExit: EAX = unit, ECX = game; void. 10331(unit) gives a target; 10826(target) must be 0x78.
// Then the type-0x23 quest node (game+0x10F4 list) with byte +9 set and state st (+0x18) with st+0xC == 0, byte st+0 == 0 and byte st+0x11
// set: st+0x54 = 0, 0xDFCC0 runs (EAX 0, EBX callback module+0x57440, EDI game, stack 0); if st+0x54 is still 0, bytes st+0x11 and st+0x10
// are cleared and 0x5E0E0(game, node, st) runs.
#pragma once
#include <cstdint>

namespace d2g5e200 {
template <class E> void Run(E& e, uint32_t unit, uint32_t game, uint32_t mod = 0xB2000000u)
{
    const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!a) return;
    if (e.h(0xA556, 0, 0, 0, 0, 0, a) != 0x78) return;
    const uint32_t lst = e.rd32(game + 0x10F4);
    if (!lst) return;
    uint32_t n = e.rd32(lst);
    while (n && e.rd32(n) != 0x23) n = e.rd32(n + 0xF4);
    if (!n || !e.rd8(n + 9)) return;
    const uint32_t st = e.rd32(n + 0x18);
    if (e.rd32(st + 0xC) || e.rd8(st) || !e.rd8(st + 0x11)) return;
    e.wr32(st + 0x54, 0);
    e.h(0xDFCC0, 0, 0, 0, mod | 0x57440, 0, 0); e.edi(0xDFCC0, game);
    if (e.rd32(st + 0x54)) return;
    e.wr8(e.rd32(n + 0x18) + 0x11, 0);
    e.wr8(e.rd32(n + 0x18) + 0x10, 0);
    e.h(0x5E0E0, 0, 0, 0, 0, 0, game, n, e.rd32(n + 0x18));
}
}
