// 1.13c D2Game+0x470F0 (create a randomly repeated unit from a table row), transcribed from the 1.13c disassembly. Convention:
// EAX = first value, ECX = second value, EBX = row index into the 0x1A8-stride table at dt+0xA78 (count at dt+0xA80), stack = (word, word),
// ret 8. Row bytes +0x2F (low) and +0x30 (high) must both be non-zero with high >= low, else 0. Callees as retail passes them:
// 0xAF28 (stack s1, ECX-in, EAX-in, ret 0xC; a zero result returns 0), 0xEF510 (EAX = row, ECX = 1, EDX = s1, stack s0, AF28 result,
// ECX-in, EAX-in, -1, 0; a zero result returns 0), 0x11D0 (ECX = new unit + 0x20, ESI = high - low + 1; a random number below that), then
// 0xEF570 (EAX = row, ECX = 1, EDX = new unit, stack s0, AF28 result, 3, 0) (low - 1 + that number) times. Returns the new unit from 0xEF510.
#pragma once
#include <cstdint>

namespace r470f0 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t eaxIn, uint32_t ecxIn, int32_t row, uint32_t s0, uint32_t s1)
{
    uint32_t rec = 0;
    if (row >= 0 && row < (int32_t)e.rd32(dt + 0xA80)) rec = (uint32_t)row * 0x1A8 + e.rd32(dt + 0xA78);
    if (!rec) return 0;
    const uint32_t lo = e.rd8(rec + 0x2F), hi = e.rd8(rec + 0x30);
    if (!lo || !hi || hi < lo) return 0;
    const uint32_t a = e.h(0xAF28, 0, 0, 0, 0, 0, s1, ecxIn, eaxIn);
    if (!a) return 0;
    const uint32_t u = e.h(0xEF510, (uint32_t)row, 1, s1, 0, 0, s0, a, ecxIn, eaxIn, 0xFFFFFFFFu);
    if (!u) return 0;
    const uint32_t mn = (lo - 1) & 0xFF, range = ((hi - 1) & 0xFF) - mn + 1;
    int32_t n = (int32_t)e.h(0x11D0, 0, u + 0x20, 0, 0, range) + (int32_t)mn;
    for (; n > 0; --n) e.h(0xEF570, (uint32_t)row, 1, u, 0, 0, s0, a, 3, 0);
    return u;
}
}
