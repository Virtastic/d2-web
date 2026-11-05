// Skill-hit event (two byte-identical copies, 0xA8270 and 0xA8350): ECX = game, EDX = unit, stack = command (ret 4).
// v = command[0x14]; r = 0xB1BC0(EAX = unit, ECX = game, EBX = &event; stack &v), which may rewrite v. With r != 0 and v < 0x19 (signed):
// #A496(unit, 0); event = {4, unit, r, 0...} with byte 0x15 = 0x65 when 0x1320(EAX = unit[1]) gives a row whose byte +0x104 meets
// gdwBitMasks[4], else 0x64; 0x75E10(game, &event, 1). Otherwise 0xF22C0(EAX = game, ESI = unit; 0x32). The result is that callee's EAX.
#pragma once
#include <cstdint>
#include <cstring>

namespace ga8270 {
template <class E> uint32_t Hit(E& e, uint32_t game, uint32_t unit, uint32_t cmd, uint32_t bitMasks)
{
    // 1.13c D2Game+0xA8270
    // 1.13c D2Game+0xA8350
    uint32_t ev[8];
    uint32_t v = e.rd32(cmd + 0x14);
    const uint32_t r = e.h(0xB1BC0, unit, game, 0, e.stk(ev), 0, e.stk(&v));
    if (!r || (int32_t)v >= 0x19) return e.h(0xF22C0, game, 0, 0, 0, unit, 0x32);
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    memset(ev, 0, sizeof ev);
    ev[0] = 4; ev[1] = unit;
    uint8_t kind = 0x64;
    if (unit) {
        const uint32_t row = e.h(0x1320, e.rd32(unit + 4));
        if (row && (e.rd8(row + 0x104) & e.rd32(bitMasks + 0x10))) kind = 0x65;
    }
    ((uint8_t*)ev)[0x15] = kind;
    ev[2] = r;
    const uint32_t res = e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(ev), 1);
    e.blk(0x75E10, ev, 8);
    return res;
}
}
