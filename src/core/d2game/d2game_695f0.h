// 1.13c D2Game+0x695F0: EAX = #10920 argument, ESI = key, stack = unit (ret 4). Written from the 1.13c
// disassembly as a rule over E (h = hooked callee). Rule: D2Common #10920 (0xA4C0, stdcall EAX) returns a record; its slot
// [0x60 + 4 * unit.byte[0x6D]] is a double pointer p. The (key, value) table at 0x107E50 (count at 0x107F68, signed) is scanned for
// ESI; the match's value, or 0, goes with **p to Fog #10118 (0xA310, stdcall 2), whose EAX is the result.
#pragma once
#include <cstdint>

namespace g695f0 {
template <class E> uint32_t ObjectSkillLookup(E& e, uint32_t eax, uint32_t key, uint32_t unit, uint32_t tbl, int32_t count)
{
    // 1.13c D2Game+0x695F0
    const uint32_t rec = e.h(0xA4C0, 0, 0, 0, 0, 0, eax);
    const uint32_t v = e.rd32(rec + 0x60 + 4 * e.rd8(unit + 0x6D));
    uint32_t val = 0;
    for (int32_t i = 0; i < count; ++i)
        if (e.rd32(tbl + 8 * i) == key) { val = e.rd32(tbl + 8 * i + 4); break; }
    return e.h(0xA310, 0, 0, 0, 0, 0, e.rd32(e.rd32(v)), val);
}
}
