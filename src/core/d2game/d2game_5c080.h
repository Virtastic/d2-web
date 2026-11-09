// 1.13c D2Game+0x5C080 ACT5Q5_TryRewardOnClass4: ECX = quest record, EDX = player unit, one unused stack word (ret 4); void.
// st = the player's quest-flag record for difficulty quest+0x6D (Ordinal 10920 pdata, +0x10 + 4*diff). Flag (0x27, 0) already set: done.
// Otherwise Ordinal 10587(unit, 0xC, 0) (signed) must reach (diff+1)*20, Ordinal 10331(unit) and 10826 must be nonzero and 10459's
// low byte 4; then flags (0x27, 0) and (0x27, 0xD) are set by Ordinal 10003 and 0x5BF70 runs (EAX = unit, stack quest).
#pragma once
#include <cstdint>

namespace d2g5c080 {
template <class E> void Run(E& e, uint32_t q, uint32_t unit)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t st = e.rd32(pd + 0x10 + 4 * e.rd8(q + 0x6D));
    if (e.h(0xA568, 0, 0, 0, 0, 0, st, 0x27, 0)) return;
    const int32_t lvl = (int32_t)e.h(0xA3E2, 0, 0, 0, 0, 0, unit, 0xC, 0);
    if (lvl < (int32_t)((e.rd8(q + 0x6D) + 1) * 20)) return;
    const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!a) return;
    const uint32_t b = e.h(0xA556, 0, 0, 0, 0, 0, a);
    if (!b) return;
    if ((e.h(0xA56E, 0, 0, 0, 0, 0, b) & 0xFF) != 4) return;
    e.h(0xA57A, 0, 0, 0, 0, 0, st, 0x27, 0);
    e.h(0xA57A, 0, 0, 0, 0, 0, st, 0x27, 0xD);
    e.h(0x5BF70, unit, 0, 0, 0, 0, q);
}
}
