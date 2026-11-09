// 1.13c D2Game+0x3EAE0 (a skill hit callback): ecx = pGame, edx = unit, stack = target unit, ret 4.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape). Rule: s = import 0xB26A(unit), l = import
// 0xB0FC(unit); s outside [0, skills count), a null skill record, a null unit or unit +0xC8 bit 10 clear -> 1. o = 0xE03A0 (eax = unit
// +0x94, ecx = game, edx = unit +0x98; plain ret); null -> 1. A target of type 1: k = record.s16[+0x82]; k negative or not below
// dt +0xC4 -> 1; import 0xA68E(target, k) zero: a = import 0xA42A(o, 0xC, 0), b = 0x1BC0(eax = s; plain ret); b + a + l above import
// 0xA42A(target, 0xC, 0) (signed) -> 0xB2120 (eax = o, ecx = game, stack 0x1A90(eax = s, edx = l; plain ret); ret 4). Result 0.
#pragma once
#include <cstdint>

namespace d2g3eae0 {
template <class E> uint32_t SkillHit(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t target)
{
    const int32_t s = (int32_t)e.h(0xB26A, 0, 0, 0, 0, 0, unit);
    const uint32_t l = e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    if (s < 0 || s >= (int32_t)e.rd32(dt + 0xBA0)) return 1;
    const uint32_t rec = (uint32_t)s * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec || !unit || !((e.rd32(unit + 0xC8) >> 10) & 1)) return 1;
    const uint32_t o = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
    if (!o) return 1;
    if (!target || e.rd32(target) != 1) return 0;
    const int32_t k = e.rd16s(rec + 0x82);
    if (k < 0 || k >= (int32_t)e.rd32(dt + 0xC4)) return 1;
    const uint32_t z = e.h(0xA68E, 0, 0, 0, 0, 0, target, (uint32_t)k);
    if (z) return 0;
    const uint32_t a = e.h(0xA42A, 0, 0, 0, 0, 0, o, 0xC, z);
    const uint32_t b = e.h(0x1BC0, (uint32_t)s);
    const int32_t sum = (int32_t)(b + a + l);
    if (sum > (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, target, 0xC, 0)) {
        const uint32_t v = e.h(0x1A90, (uint32_t)s, 0, l);
        e.h(0xB2120, o, game, 0, 0, 0, v);
    }
    return 0;
}
}
