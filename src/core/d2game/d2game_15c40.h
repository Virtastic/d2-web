// 1.13c D2Game+0x15C40 (AI unit-scan callback, counts and ranks class 0x1C5 monsters): ECX = game argument, EDX = scanning unit,
// stack = (target, acc) ret 8; acc = { +0 maxDistA, +4 maxDistB, +8 levelCap, +0xC best1, +0x10 best2, +0x14 count }.
// Rule: not the unit itself, a non-null monster; class (0x18A0, EAX = target) within 0..monstats count (dt+0xA80) and == 0x1C5;
// 0xA688(target) == 0; 0xE0A60 (EAX = target, ECX = unit, stack = game argument; ret 4) == 0; 0xA68E(target, 0x8D) == 0.
// Then count++; best1 (if empty): 0x1900 (EAX = unit, EBX = target) <= acc[0] and 0xA718(target) < acc[2] sets best1 = target;
// best2 (if empty): 0x1900 <= acc[1] and 0x22A80 (EAX = target, stack 8; ret 4) == 0 sets best2 = target. Result 0.
#pragma once
#include <cstdint>

namespace g15c40 {
template <class E> uint32_t RankCallback(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t tgt, uint32_t acc)
{
    // 1.13c D2Game+0x15C40
    if (unit == tgt || !tgt || e.rd32(tgt) != 1) return 0;
    const int32_t c = (int32_t)e.h(0x18A0, tgt);
    if (c < 0 || c >= (int32_t)e.rd32(dt + 0xA80) || c != 0x1C5) return 0;
    if (e.h(0xA688, 0, 0, 0, 0, 0, tgt)) return 0;
    if (e.h(0xE0A60, tgt, unit, 0, 0, 0, game)) return 0;
    if (e.h(0xA68E, 0, 0, 0, 0, 0, tgt, 0x8D)) return 0;
    e.wr32(acc + 0x14, e.rd32(acc + 0x14) + 1);
    if (e.rd32(acc + 0xC) == 0) {
        if ((int32_t)e.h(0x1900, unit, 0, 0, tgt) <= (int32_t)e.rd32(acc)) {
            if ((int32_t)e.h(0xA718, 0, 0, 0, 0, 0, tgt) < (int32_t)e.rd32(acc + 8)) e.wr32(acc + 0xC, tgt);
        }
    }
    if (e.rd32(acc + 0x10) == 0) {
        if ((int32_t)e.h(0x1900, unit, 0, 0, tgt) <= (int32_t)e.rd32(acc + 4)) {
            if (e.h(0x22A80, tgt, 0, 0, 0, 0, 8) == 0) e.wr32(acc + 0x10, tgt);
        }
    }
    return 0;
}
}
