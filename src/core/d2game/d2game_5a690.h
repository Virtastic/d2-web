// 1.13c D2Game+0x5A690 (ACT5Q1_SetNodeState2OnUnit): eax = pGame, edx = pUnit, plain ret, no result.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape). Rule: walk the quest node list
// (**(game+0x10F4), next at node+0xF4) to the first node of type 0x1F (none -> return). The node must have byte +9 set and
// byte +0xB below 2; the unit must be non-null, of type 1, with a +0x14 record whose word +0x26 is 0x2A. Then node byte
// +0x14 := 0, +0xB := 2, and 0xDFCC0 is called with eax = 0, ebx = the callback at module RVA 0x59620, edi = node's +4
// word (the game), stack 0 (ret 4).
#pragma once
#include <cstdint>

namespace d2g5a690 {
template <class E> void SetNodeState2(E& e, uint32_t game, uint32_t unit)
{
    const uint32_t ctrl = e.rd32(game + 0x10F4);
    if (!ctrl) return;
    uint32_t node = e.rd32(ctrl);
    if (!node) return;
    while (e.rd32(node) != 0x1F) {
        node = e.rd32(node + 0xF4);
        if (!node) return;
    }
    if (e.rd8(node + 9) == 0 || e.rd8(node + 0xB) >= 2) return;
    if (!unit || e.rd32(unit) != 1) return;
    const uint32_t rec = e.rd32(unit + 0x14);
    if (!rec || (e.rd16s(rec + 0x26) & 0xFFFF) != 0x2A) return;
    e.wr8(node + 0x14, 0);
    e.wr8(node + 0xB, 2);
    const uint32_t edi = e.rd32(node + 4);
    e.h(0xDFCC0, 0, 0, 0, 0xB2059620u, 0, 0);
    e.edi(0xDFCC0, edi);
}
}
