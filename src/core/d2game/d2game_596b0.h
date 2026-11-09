// 1.13c D2Game+0x596B0 (ACT5Q4 object-operate callback): ecx = quest node, edx = callback args, plain ret, no result.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape). Rule: code1 = the args' +8 record word +4
// (no record -> -1), code2 = the +0xC record word +4 (-1). code1 - 0x200 picks (want1, want2) from the jump table: 0 and 8 -> (7, 7),
// 1 -> (4, 1), 2 -> (6, 2), 3 -> (5, 3), anything else returns. 0x69590 (stack args word 0, ret 4) nonzero returns. Then 0x33EB0 is
// called with ecx = node, stack args word +0x14 and code1 (ret 8), eax = 1 when code2 == want1, else 2 when code2 == want2, else 0.
#pragma once
#include <cstdint>

namespace d2g596b0 {
template <class E> void Operate(E& e, uint32_t node, uint32_t msg)
{
    const uint32_t r1 = e.rd32(msg + 8), r2 = e.rd32(msg + 0xC);
    const uint32_t code1 = r1 ? e.rd32(r1 + 4) : 0xFFFFFFFFu;
    const uint32_t code2 = r2 ? e.rd32(r2 + 4) : 0xFFFFFFFFu;
    uint32_t w1 = 7, w2 = 7;
    switch (code1 - 0x200) {
    case 0: case 8: break;
    case 1: w1 = 4; w2 = 1; break;
    case 2: w1 = 6; w2 = 2; break;
    case 3: w1 = 5; w2 = 3; break;
    default: return;
    }
    if (e.h(0x69590, 0, 0, 0, 0, 0, e.rd32(msg))) return;
    const uint32_t eax = code2 == w1 ? 1u : code2 == w2 ? 2u : 0u;
    e.h(0x33EB0, eax, node, 0, 0, 0, e.rd32(msg + 0x14), code1);
}
}
