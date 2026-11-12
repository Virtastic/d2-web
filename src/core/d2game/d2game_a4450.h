// 1.13c D2Game+0xA4450: AI init callback "shadow warrior" (table entry). edx = the unit, eax as entered, stack:
// the AI parameter block ([0] the AI state, [0x1C] the monster's stats record) (ret 4). The state's word at
// +0x1C becomes 1. For a monster (type 1 at +0x00) whose data (+0x14) has an AI control (+0x28) with an owner
// game word (+0x28), the owner is looked up (0xE03A0: eax = control +0x30, ecx = control +0x28, edx = control
// +0x2C); with an owner, its skill of the stats record's id (signed word at +0x80) is looked up (0xA3F4: ecx =
// owner, edx = id) and, when it has one, the state's word becomes that skill's level (0xA400: owner, skill, 1).
// The result is whatever the taken path left in eax: eax as entered without a unit, the unit's type when it is
// no monster, 1 without monster data, 0 without a control, the control when it has no game word, else the last
// callee's answer.
#pragma once
#include <cstdint>

namespace ga4450 {
template <class E> uint32_t Init(E& e, uint32_t unit, uint32_t eax, uint32_t p)
{
    const uint32_t st = e.rd32(p);
    e.wr32(st + 0x1C, 1);
    if (!unit) return eax;
    if (e.rd32(unit) != 1) return e.rd32(unit);
    const uint32_t d = e.rd32(unit + 0x14);
    if (!d) return 1;
    const uint32_t ai = e.rd32(d + 0x28);
    if (!ai) return 0;
    if (!e.rd32(ai + 0x28)) return ai;
    const uint32_t owner = e.h(0xE03A0, e.rd32(ai + 0x30), e.rd32(ai + 0x28), e.rd32(ai + 0x2C));
    if (!owner) return 0;
    const uint32_t rec = e.rd32(p + 0x1C);
    const uint32_t sk = e.h(0xA3F4, 0, owner, (uint32_t)e.rd16s(rec + 0x80));
    if (!sk) return 0;
    const uint32_t lvl = e.h(0xA400, 0, 0, 0, 0, 0, owner, sk, 1);
    e.wr32(st + 0x1C, lvl);
    return lvl;
}
}
