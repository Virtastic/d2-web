// 1.13c D2Game+0xB9E80: object init callback of the fourth act's quest object (table entry). ecx = the init
// block ([0] game, [4] the object). Without the quest record 0x18 (d2game_objinit.h), or with its flag byte
// (+0x09) clear, the object goes to mode 3 (0xA4A2: object, mode) and that answer is the result. Otherwise the
// object goes to the mode in the data's 16-bit word +0x00. With the record's state byte (+0x0B) at 0x0D, 2 or 3
// the result is that answer with its low byte replaced by the state. Otherwise byte +0x14 is cleared, the state
// becomes 2 and the players are walked (0xDFCC0: eax = 0, ebx = the handler, edi = the record's game (+0x04);
// stack: 0), whose answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace gb9e80 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x18);
    if (!q || !e.rd8(q + 9)) return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 3);
    const uint32_t r = e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), e.rd32(e.rd32(q + 0x18)) & 0xFFFF);
    const uint32_t st = e.rd8(q + 0xB);
    if (st == 0xD || st == 2 || st == 3) return (r & 0xFFFFFF00u) | st;
    e.wr8(q + 0x14, 0);
    e.wr8(q + 0xB, 2);
    const uint32_t w = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xB9970, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    return w;
}
}
