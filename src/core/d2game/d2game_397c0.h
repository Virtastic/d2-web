// 1.13c D2Game+0x397C0 (MISSMODE_ResetGuidedFlight): esi = missile unit, stack = target unit, ret 4, no result.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape). Rule: a null unit or class (+4) outside
// [0, missiles count) returns; rec = class * 0x1A4 + missiles base (null returns). A target for which 0xDFA90 (eax = target; plain ret) is
// nonzero is dropped. v = (import 0xB0FC(unit) - 1) * rec.s16[+0x98] + rec.s16[+0x96]; imports 0xAD42(unit, v), 0xAC7C(unit, v);
// path = unit +0x2C. With a target: 0xA5FE(path, target), 0xA50E(unit, 5), r = 0xAB86(unit, target). Without: xy = 0xB288(unit);
// x = (int16)xy + (type 2/4/5 ? path +0xC : path ? 0xA454(path) : 0), y = (int16)(xy >> 16) + (type 2/4/5 ? path +0x10 : path ?
// 0xA3B8(path) : 0); 0xAB2C(path, x, y), 0xA50E(unit, 6), r = 0xAB80(unit, x, y). r below 0x19 (signed): 0xAAF0(path, unit, 0).
#pragma once
#include <cstdint>

namespace d2g397c0 {
template <class E> void ResetGuidedFlight(E& e, uint32_t dt, uint32_t unit, uint32_t target)
{
    if (!unit) return;
    const int32_t cls = (int32_t)e.rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)e.rd32(dt + 0xB6C)) return;
    const uint32_t rec = (uint32_t)cls * 0x1A4u + e.rd32(dt + 0xB64);
    if (!rec) return;
    if (target && e.h(0xDFA90, target)) target = 0;
    const int32_t lvl = (int32_t)e.h(0xB0FC, 0, 0, 0, 0, 0, unit);
    const uint32_t v = (uint32_t)((lvl - 1) * e.rd16s(rec + 0x98) + e.rd16s(rec + 0x96));
    e.h(0xAD42, 0, 0, 0, 0, 0, unit, v);
    e.h(0xAC7C, 0, 0, 0, 0, 0, unit, v);
    const uint32_t path = e.rd32(unit + 0x2C);
    int32_t r;
    if (target) {
        e.h(0xA5FE, 0, 0, 0, 0, 0, path, target);
        e.h(0xA50E, 0, 0, 0, 0, 0, unit, 5);
        r = (int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, unit, target);
    } else {
        const uint32_t xy = e.h(0xB288, 0, 0, 0, 0, 0, unit);
        auto direct = [&]() { const uint32_t t = e.rd32(unit); return t == 2 || t == 4 || t == 5; };
        uint32_t x0, y0;
        if (direct()) x0 = e.rd32(e.rd32(unit + 0x2C) + 0xC);
        else { const uint32_t p = e.rd32(unit + 0x2C); x0 = p ? e.h(0xA454, 0, 0, 0, 0, 0, p) : 0; }
        const uint32_t x = (uint32_t)(int32_t)(int16_t)(xy & 0xFFFF) + x0;
        if (direct()) y0 = e.rd32(e.rd32(unit + 0x2C) + 0x10);
        else { const uint32_t p = e.rd32(unit + 0x2C); y0 = p ? e.h(0xA3B8, 0, 0, 0, 0, 0, p) : 0; }
        const uint32_t y = (uint32_t)(int32_t)(int16_t)(xy >> 16) + y0;
        e.h(0xAB2C, 0, 0, 0, 0, 0, path, x, y);
        e.h(0xA50E, 0, 0, 0, 0, 0, unit, 6);
        r = (int32_t)e.h(0xAB80, 0, 0, 0, 0, 0, unit, x, y);
    }
    if (r < 0x19) e.h(0xAAF0, 0, 0, 0, 0, 0, path, unit, 0);
}
}
