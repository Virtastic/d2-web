// Nearby-object trigger: EAX = unit, ECX = target, EDI = game (plain ret). Needs the unit-data byte +0xC to share a bit with gdwBitMasks[3]
// and #AF2E(target[0x2C]) to have bit 0x800; then 0xB17B0 (EAX = 8, ECX = &{0, 9}, EDX = game; stack target, 0) finds an object.
// A found object whose row #A520(found[1]) has byte +0x16D set gets 0x6A720 (EAX = 2, ECX = target; stack game, found[3]) and
// 0xF22C0 (EAX = game, ESI = target; 5): result 1. Otherwise 0.
#pragma once
#include <cstdint>

namespace g9ccf0 {
template <class E> uint32_t Trigger(E& e, uint32_t unit, uint32_t tgt, uint32_t game, uint32_t bitMasks)
{
    // 1.13c D2Game+0x9CCF0
    if (!(e.rd8(bitMasks + 0xC) & e.rd8(e.rd32(unit + 0x1C) + 0xC))) return 0;
    if (!(e.h(0xAF2E, 0, 0, 0, 0, 0, e.rd32(tgt + 0x2C)) & 0x800)) return 0;
    uint32_t loc[2] = { 0, 9 };
    const uint32_t in[2] = { 0, 9 };
    e.h(0xB17B0, 8, e.stk(loc), game, 0, 0, tgt, 0);
    e.blk(0xB17B0, in, 2);   // retail logs the block at hook entry, before the hook's out-word lands
    const uint32_t found = loc[0];
    if (!found) return 0;
    const uint32_t row = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(found + 4));
    if (!e.rd8(row + 0x16D)) return 0;
    e.h(0x6A720, 2, tgt, 0, 0, 0, game, e.rd32(found + 0xC));
    e.h(0xF22C0, game, 0, 0, 0, tgt, 5);
    return 1;
}
}
