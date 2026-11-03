// 1.13c D2Game+0x22BC0: ECX = unit, stack = param (ret 4). The rule, shared by the engine port
// (src/core/d2game/d2game_22bc0.cpp) and the FN probe (src/core/fn/d2game_22bc0.cpp). dt = *sgptDataTables (slot RVA 0xF829C).
// Callees through the env: h(0xA7BA, stack: difficulty) returns the difficulty record (+0x34 percent); h(0x18A0, EAX = unit) returns the class;
// h(0xA4D2, stack: unit, stat, value, layer) adds a stat.
#pragma once
#include <cstdint>

namespace g22bc0 {
// Field +0x1c of row `row` of the 0x20-byte table at dt+0xc50 (count dt+0xc54): 0 when the table is too short or the row pointer is 0.
template <class E> int32_t rowField(E& e, uint32_t dt, int32_t row)
{
    if ((int32_t)e.rd32(dt + 0xc54) <= row) return 0;
    const uint32_t p = e.rd32(dt + 0xc50) + (uint32_t)row * 0x20u;
    return p ? (int32_t)e.rd32(p + 0x1c) : 0;
}
template <class E> void Fn(E& e, uint32_t unit, int32_t param, uint32_t dt)
{
    if (!unit || e.rd32(unit) != 1) return;
    const uint32_t pl = e.rd32(unit + 0x80);
    if (!pl) return;
    int32_t a = rowField(e, dt, param ? 15 : 14);
    int32_t b = rowField(e, dt, param ? 13 : 12);
    const uint32_t row = e.h(0xA7BA, 0, 0, 0, 0, 0, e.rd8(pl + 0x6d));
    if (row) {
        const int32_t m = (int32_t)e.rd32(row + 0x34);
        a = (int32_t)((uint32_t)m * (uint32_t)a) / 100;
        b = (int32_t)((uint32_t)m * (uint32_t)b) / 100;
        const int32_t cls = (int32_t)e.h(0x18A0, unit);
        if (cls >= 0 && cls < (int32_t)e.rd32(dt + 0xa80) && cls == 0x76) a = a / 2;   // cdq; sub; sar 1: truncating halve
    }
    e.h(0xA4D2, 0, 0, 0, 0, 0, unit, 0x19, (uint32_t)a, 0);
    e.h(0xA4D2, 0, 0, 0, 0, 0, unit, 0x77, (uint32_t)b, 0);
}
}
