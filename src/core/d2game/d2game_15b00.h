// 1.13c D2Game+0x15B00 (AI unit-scan callback, siege beast: pick a target of class 0x1EC): ECX = game argument, EDX = scanning unit,
// stack = (target, &limit) ret 8. Rule: not the unit itself, a non-null monster; 0xA688(target) must be 0; class (0x18A0, EAX = target,
// no range check) must equal 0x1EC when the monstats count (dt+0xA80) > 0x1EC, else -1; 0xA6F4(unit, target) nonzero; 0xA514(target) == 0;
// 0xE0A20 (EAX = unit, stack = ECX argument; ret 4) == 0; the target's AI block (0x18E0, EAX = target) +0x14 == -1; then 0x1900
// (EAX = unit, EBX = target) is compared with *limit: the target is returned unless the distance is larger. Else 0.
#pragma once
#include <cstdint>

namespace g15b00 {
template <class E> uint32_t SiegeBeastCallback(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t tgt, uint32_t limit)
{
    // 1.13c D2Game+0x15B00
    if (unit == tgt || !tgt || e.rd32(tgt) != 1) return 0;
    if (e.h(0xA688, 0, 0, 0, 0, 0, tgt)) return 0;
    const int32_t want = e.rd32(dt + 0xA80) > 0x1EC ? 0x1EC : -1;
    if ((int32_t)e.h(0x18A0, tgt) != want) return 0;
    if (!e.h(0xA6F4, 0, 0, 0, 0, 0, unit, tgt)) return 0;
    if (e.h(0xA514, 0, 0, 0, 0, 0, tgt)) return 0;
    if (e.h(0xE0A20, unit, 0, 0, 0, 0, game)) return 0;
    const uint32_t ai = e.h(0x18E0, tgt);
    if (e.rd32(ai + 0x14) != 0xFFFFFFFFu) return 0;
    const int32_t d = (int32_t)e.h(0x1900, unit, 0, 0, tgt);
    return d > (int32_t)e.rd32(limit) ? 0 : tgt;
}
}
