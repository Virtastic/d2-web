// 1.13c D2Game+0x47880 (spawn a random 1-3 of a row near a monster), transcribed from the 1.13c disassembly. Convention: EBX = row,
// ESI = unit, EDI = value, stack = (word), ret 4; EAX is passed through untouched on the early exits (the probe carries it in).
// The unit must be non-null, type 1 (monster) and have a non-null data pointer (unit+0x14) whose byte +0x16 has bit 4 set; the result
// on each early exit is what EAX holds there: EAX-in, the unit type, 0, the data pointer. Then the unit's LCG (unit+0x20 low, +0x24 high,
// multiplier 0x6AC690C5, the new low word is the output) advances once and n = low % 3 + 1. n times: 0xEF570 (EAX = row, ECX = 1,
// EDX = unit, stack value, word, 4, 0; ret 0x10); when it is non-zero 0x24BF0 (ECX = value, stack 0x10; ret 4). The result is the last callee's EAX.
#pragma once
#include <cstdint>

namespace r47880 {
template <class E> uint32_t Do(E& e, uint32_t eaxIn, uint32_t row, uint32_t unit, uint32_t val, uint32_t w)
{
    if (!unit) return eaxIn;
    const uint32_t type = e.rd32(unit);
    if (type != 1) return type;
    const uint32_t p = e.rd32(unit + 0x14);
    if (!p) return 0;
    if (!(e.rd8(p + 0x16) & 4)) return p;
    const uint64_t m = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x24, (uint32_t)(m >> 32));
    e.wr32(unit + 0x20, (uint32_t)m);
    uint32_t last = 0;
    for (uint32_t n = (uint32_t)m % 3 + 1; n; --n) {
        last = e.h(0xEF570, row, 1, unit, 0, 0, val, w, 4, 0);
        if (last) last = e.h(0x24BF0, 0, val, 0, 0, 0, 0x10);
    }
    return last;
}
}
