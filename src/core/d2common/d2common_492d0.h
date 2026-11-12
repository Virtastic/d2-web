// 1.13c D2Common+0x492D0: item property callback "set the fixed minimum damage" (table entry). ecx and edx = the
// two register words, nine stack words (ret 0x24): the item, the property record, words 2 and 3 (3 = the stat),
// ..., words 6, 7 and 8 passed on. No record: 0. The value is record +0x08. When the low 16 bits of the stat are
// 0x15 the damage helper applies it (0x487C0: ecx, edx as entered; stack: item, record, word 2, 0x15, 0, the
// value, words 6, 7 and 8); otherwise the plain apply does (0x475E0: eax = record; stack: edx word, item, words 2
// and 3, 0, the value, words 6 and 7). The result is the value.
#pragma once
#include <cstdint>

namespace c492d0 {
template <class E> uint32_t Apply(E& e, uint32_t c, uint32_t d, uint32_t item, uint32_t rec, uint32_t s2, uint32_t s3, uint32_t s6, uint32_t s7, uint32_t s8)
{
    if (!rec) return 0;
    const uint32_t val = e.rd32(rec + 8);
    if ((s3 & 0xFFFF) == 0x15) {
        e.h(0x487C0, 0, c, d, 0, 0, item, rec, s2, 0x15);
        const uint32_t w[5] = { 0, val, s6, s7, s8 };
        e.x(0x487C0, w, 5);
    } else {
        e.h(0x475E0, rec, 0, 0, 0, 0, d, item, s2, s3);
        const uint32_t w[4] = { 0, val, s6, s7 };
        e.x(0x475E0, w, 4);
    }
    return val;
}
}
