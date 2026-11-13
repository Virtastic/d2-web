// 1.13c D2Game+0x25E60: event-type dispatcher, ECX = game, EDX = event (null allowed), two unused stack words (ret 8).
// Reads type = ev[1], flag = ev[4]; acts only when flag == 0 (a null event does nothing):
//   0x9C -> 0xEA730(game, ev, 0x23, 1, 0x33, 0)      0xE5 -> 0xEA930(EAX = ev, EBX = game)     0xF2 -> 0xEAB10(EDX = ev, [stk] = game)
//   0x100 -> 0xEA730(game, ev, 0x69, 1, 2, 0)        0x10B -> 0xEA730(game, ev, 0x23, 0x19, 0x7D, 1)
//   0x1DF -> 0xEA730(game, ev, 0x23, 0x19, 0x7D, 0)  0x2C0/0x2C1/0x2C5 -> 0xEAA30(EAX = game, ESI = ev)
// Every other type returns. The retail extent is 0x25E60..0x25F5C (switch tables after it); the decompiler cut it at 0x25EC9.
#pragma once
#include <cstdint>
namespace g25e60 {
template <class E> void Run(E& e, uint32_t game, uint32_t ev)
{
    // 1.13c D2Game+0x25E60
    if (!ev) return;
    const uint32_t t = e.rd32(ev + 4);
    if (e.rd32(ev + 0x10)) return;
    auto k = [&](uint32_t c, uint32_t d, uint32_t f, uint32_t g) {
        const uint32_t a[6] = { game, ev, c, d, f, g };
        e.h(0xEA730, 0, 0, 0, 0, 0, a[0], a[1], a[2], a[3]); e.blk(0xEA730, a, 6);
    };
    switch (t) {
    case 0x9C:  k(0x23, 1, 0x33, 0); break;
    case 0xE5:  e.h(0xEA930, ev, 0, 0, game, 0); break;
    case 0xF2:  e.h(0xEAB10, 0, 0, ev, 0, 0, game); break;
    case 0x100: k(0x69, 1, 2, 0); break;
    case 0x10B: k(0x23, 0x19, 0x7D, 1); break;
    case 0x1DF: k(0x23, 0x19, 0x7D, 0); break;
    case 0x2C0: case 0x2C1: case 0x2C5: e.h(0xEAA30, game, 0, 0, 0, ev); break;
    }
}
}
