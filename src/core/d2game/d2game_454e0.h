// 1.13c D2Game+0x454E0 (repeat a unit-bound missile request), transcribed from the 1.13c disassembly. Convention: EAX = flag,
// ECX = value kept in the request, EDX = unit, stack = (game, count, word, word), ret 0x10; always returns 1. With the flag set, 0xA28F0
// (ECX = game, EDX = third word, stack = unit, last word, ECX-in, five zeros) runs once and the count drops by one; nothing more happens
// when the count is then 0 or less. The 23-dword request is zeroed, then {0x21, unit, ., ., third word, unit-type value A, unit-type
// value B, out0, out1, ., ., last word, ECX-in, ., ., ., ., ., loop index at 21}. A and B come from 0xA454 / 0xA3B8 of the unit's +0x2C
// pointer (0 if none) for unit types 0, 1, 3 and above 5, and from the words at +0xC / +0x10 of that pointer for types 2, 4 and 5.
// 0x9FA30 (EAX = unit, ECX = game, EBX = &out0, stack word 0 = &out1) fills out0/out1 in the request; when it returns 0 the loop is
// skipped. Then 0x6F930 (ECX = game, EDX = request) runs once per remaining count with the loop index in word 21.
#pragma once
#include <cstdint>

namespace r454e0 {
template <class E> uint32_t Do(E& e, uint32_t flag, uint32_t ecxIn, uint32_t unit, uint32_t game, int32_t count, uint32_t w2, uint32_t w3)
{
    if (flag) {
        e.h(0xA28F0, 0, game, w2, 0, 0, unit, w3, ecxIn, 0);
        if (--count <= 0) return 1;
    }
    uint32_t b[23] = {};
    b[0] = 0x21;
    b[1] = unit;
    const int32_t ty = (int32_t)e.rd32(unit);
    const bool direct = ty == 2 || (ty > 3 && ty <= 5);
    const uint32_t p = e.rd32(unit + 0x2C);
    b[5] = direct ? e.rd32(p + 0xC) : (p ? e.h(0xA454, 0, 0, 0, 0, 0, p) : 0);
    b[6] = direct ? e.rd32(p + 0x10) : (p ? e.h(0xA3B8, 0, 0, 0, 0, 0, p) : 0);
    b[4] = w2;
    b[11] = w3;
    b[12] = ecxIn;
    b[21] = 0;
    if (!e.h(0x9FA30, unit, game, 0, e.stk(&b[7]), 0, e.stk(&b[8]))) return 1;
    for (int32_t i = 0; i < count; ++i) {
        b[22] = (uint32_t)i;
        e.h(0x6F930, 0, game, e.stk(b));
        e.blk(0x6F930, b, 23, 0x5FFFFFu);
    }
    return 1;
}
}
