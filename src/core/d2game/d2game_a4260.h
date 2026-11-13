// 1.13c D2Game+0xA4260: monster AI callback, the shadow master's attack-skill setup (table entry). ecx = game
// (unused), edx = unit, stack: the parameter block. The control's word +0x1C becomes 1. For a monster (type 1)
// whose data (+0x14) has an owner record x (+0x28) with a game (x +0x28), the owner is looked up (0xE03A0: eax
// = x +0x30, ecx = x +0x28, edx = x +0x2C); when it is a player (type 0) the skill of the record's word +0x80
// is looked up on it (0xA3F4: ecx = owner, edx = skill) and, when there, its level (0xA400: owner, skill, 1)
// goes to the control word. Then the unit's stat list is asked for (0xA3D0: ecx = unit, edx 0; stack -1),
// made when missing (0xA9FA: unit, 0, 1, 0, the source file name, line 0xC1D), and two unit settings are reset
// (0xADF0, 0xAE08: unit, 0, -1). EAX is what the last step left: the unit's type when not 1, 0 without data,
// owner record or owner, else the answer of 0xAE08 (the remaining early exits leave an address).
#pragma once
#include <cstdint>

namespace ga4260 {
template <class E> uint32_t Think(E& e, uint32_t unit, uint32_t tp, uint32_t eaxIn = 0, uint32_t mod = 0xB2000000u)
{
    const uint32_t ctl = e.rd32(tp);
    e.wr32(ctl + 0x1C, 1);
    if (!unit) return eaxIn;
    if (e.rd32(unit) != 1) return e.rd32(unit);
    const uint32_t pd = e.rd32(unit + 0x14);
    if (!pd) return 0;
    const uint32_t x = e.rd32(pd + 0x28);
    if (!x) return 0;
    if (!e.rd32(x + 0x28)) return x;
    const uint32_t owner = e.h(0xE03A0, e.rd32(x + 0x30), e.rd32(x + 0x28), e.rd32(x + 0x2C));
    if (!owner || e.rd32(owner)) return owner;
    const uint32_t sk = e.h(0xA3F4, 0, owner, (uint32_t)e.rd16s(e.rd32(tp + 0x1C) + 0x80));
    if (sk) e.wr32(ctl + 0x1C, e.h(0xA400, 0, 0, 0, 0, 0, owner, sk, 1));
    if (!e.h(0xA3D0, 0, unit, 0, 0, 0, 0xFFFFFFFFu)) {
        const uint32_t a[6] = { unit, 0, 1, 0, mod | 0xFB6B0, 0xC1D };
        e.h(0xA9FA, 0, 0, 0, 0, 0, a[0], a[1], a[2], a[3], a[4], a[5]);
        e.blk(0xA9FA, a, 6);
    }
    e.h(0xADF0, 0, 0, 0, 0, 0, unit, 0, 0xFFFFFFFFu);
    return e.h(0xAE08, 0, 0, 0, 0, 0, unit, 0, 0xFFFFFFFFu);
}
}
