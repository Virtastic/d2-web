// 1.13c D2Common+0x491C0: item property callback "add a fixed or rolled stat, maximum damage through the damage
// helper" (table entry). ecx and edx = the two register words, nine stack words (ret 0x24): the item, the property
// record, words 2 and 3 (3 = the stat), ..., words 6, 7 and 8 passed on. No record: 0. The value is record +0x04;
// zero is replaced by a roll (0x465B0: eax = record +0x0C, ecx = record +0x08, edx = item) and a zero roll ends
// with 0. When the low 16 bits of the stat are 0x16 the damage helper applies it (0x48630: ecx, edx as entered;
// stack: item, record, word 2, 0x16, 0, the value, words 6, 7 and 8); otherwise the plain apply does (0x475E0: eax =
// record; stack: edx word, item, words 2 and 3, 0, the value, words 6 and 7). The result is the value.
#pragma once
#include <cstdint>

namespace c491c0 {
template <class E> uint32_t Apply(E& e, uint32_t c, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t s6, uint32_t s7, uint32_t s8)
{
    if (!rec) return 0;
    uint32_t val = e.rd32(rec + 4);
    if (!val) { val = e.h(0x465B0, e.rd32(rec + 0xC), e.rd32(rec + 8), item); if (!val) return 0; }
    if ((s3 & 0xFFFF) == 0x16) {
        e.h(0x48630, 0, c, d, 0, 0, item, rec, s2, 0x16);
        const uint32_t w[5] = { 0, val, s6, s7, s8 };
        e.x(0x48630, w, 5);
    } else {
        e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3);
        const uint32_t w[4] = { 0, val, s6, s7 };
        e.x(0x475E0, w, 4);
    }
    return val;
}
}
