// 1.13c D2Game+0x6BCB0 (AITHINK_Mode0Mode2Step): ecx = think context {+0 game, +4 unit, +8 argument of the node call}, plain ret, returns 1.
// Written from the 1.13c disassembly as a rule over an environment E: every callee that is not a field read is one E::h(rva, ...) call.
// Four unit calls first (room 0xA478, position 0xA544 into a local, 0xA71E, 0xB13E). Then by the unit's mode (a null unit counts as 0):
// mode 0 calls 0xA4A2(unit, 2); mode 2 walks room+0x74's list (next at +0xE8) to the first node whose first word is 5 and calls 0xE1C00
// (eax = node, esi = ctx+8, stack = game); any other mode does nothing. A list without such a node is the fatal 0x40E (not driven).
#pragma once
#include <cstdint>

namespace r6bcb0 {
template <class E> uint32_t Step(E& e, uint32_t ctx)
{
    uint32_t pos[2] = {0, 0};
    uint32_t unit = e.rd32(ctx + 4);
    e.h(0xA478, 0, 0, 0, 0, 0, unit);
    e.h(0xA544, 0, 0, 0, 0, 0, e.rd32(ctx + 4), e.stk(pos));
    e.h(0xA71E, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    e.h(0xB13E, 0, 0, 0, 0, 0, e.rd32(ctx + 4));
    unit = e.rd32(ctx + 4);
    const uint32_t mode = unit ? e.rd32(unit + 0x10) : 0;
    if (mode == 0) { e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 2); return 1; }
    if (mode != 2) return 1;
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    uint32_t n = e.rd32(room + 0x74);
    while (n && e.rd32(n) != 5) n = e.rd32(n + 0xE8);
    if (!n) __builtin_trap();                       // retail: Fog 10265/10024 then _exit(-1)
    e.h(0xE1C00, n, 0, 0, 0, e.rd32(ctx + 8), e.rd32(ctx));
    return 1;
}
}
