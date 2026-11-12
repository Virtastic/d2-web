// 1.13c D2Game+0xB6790, transcribed from the 1.13c disassembly: the monster Frenzy skill server do-function.
// Convention: ECX = game, EDX = unit, stack skill id, skill level (ret 8). 0xAABA (D2Common 10511: unit) gives the pet record;
// none returns 0. 0xE07A0 (EAX = unit, ECX = game), the target from 0xA598 (unit +0x2C); none or the unit itself returns 0.
// Then 0xDA6D0 (EDI = unit; game, target), unit +0xC4 |= 0x40, 0xB2E00 (EAX = unit) and when it holds 0x9FD00 (EAX = skill;
// game, unit, level, 0); 0xB4520 (EAX = skill; game, target, level); 0xAD12 (D2Common 10980: the pet record) and when it
// holds 0x28E70 (EBX = unit; game, skill, level). Returns 1.
#pragma once
#include <cstdint>

namespace rb6790 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    const uint32_t rec = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!rec) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    e.ed(unit);
    e.h(0xDA6D0, 0, 0, 0, 0, 0, game, tgt);
    e.edi(0xDA6D0, unit);
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    if (e.h(0xB2E00, unit)) e.h(0x9FD00, skill, 0, 0, 0, 0, game, unit, level, 0);
    e.h(0xB4520, skill, 0, 0, 0, 0, game, tgt, level);
    if (e.h(0xAD12, 0, 0, 0, 0, 0, rec)) e.h(0x28E70, 0, 0, 0, unit, 0, game, skill, level);
    return 1;
}
}
