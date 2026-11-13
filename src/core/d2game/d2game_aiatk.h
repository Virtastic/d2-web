// Shared steps of the 1.13c D2Game monster AI callbacks. A callback gets ecx = game, edx = unit and one stack
// word, the parameter block: [0] the AI control, [8] the target, [0x14] the target distance, [0x18] the
// "in melee range" flag, [0x1C] the monster's stats record. The callbacks return nothing; what they leave in
// EAX is the answer of their last call, and the rules here hand that back.
#pragma once
#include <cstdint>

namespace gaiatk {
// idle for n frames (0xF22C0: eax = game, esi = unit; stack: n)
template <class E> uint32_t Idle(E& e, uint32_t game, uint32_t unit, uint32_t n)
{
    return e.h(0xF22C0, game, 0, 0, 0, unit, n);
}
// the record's word of the game's difficulty (byte +0x6D) at off
template <class E> int32_t Word(E& e, uint32_t game, uint32_t row, uint32_t off)
{
    return e.rd16s(row + e.rd8(game + 0x6D) * 2 + off);
}
// the mode command: the unit's pending action is dropped (0xA496: unit, 0) and the 8-word command
// {mode, unit, target, 0, 0, speed << 8, 0, 0} goes to 0x75E10 (stack: game, the command, 1). With bit set the
// speed byte is 0x65 instead of 0x64 when the second stats record of the unit's class (0x1320: eax = class)
// has that bit in its byte at off.
template <class E> uint32_t Mode(E& e, uint32_t game, uint32_t unit, uint32_t mode, uint32_t tgt, uint32_t off = 0, uint32_t bit = 0)
{
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t b = 0x64;
    if (bit && unit) {
        const uint32_t rec = e.h(0x1320, e.rd32(unit + 4));
        if (rec && (e.rd8(rec + off) & bit)) b = 0x65;
    }
    uint32_t blk[8] = { mode, unit, tgt, 0, 0, b << 8, 0, 0 };
    const uint32_t r = e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
    e.blk(0x75E10, blk, 8);
    return r;
}
// walk to the target (0xF2540: ecx = game, edx = unit; stack: target, 2, 0, 0, 1, 7)
template <class E> uint32_t Approach(E& e, uint32_t game, uint32_t unit, uint32_t tgt)
{
    const uint32_t a[6] = { tgt, 2, 0, 0, 1, 7 };
    const uint32_t r = e.h(0xF2540, 0, game, unit, 0, 0, a[0], a[1], a[2], a[3], a[4], a[5]);
    e.blk(0xF2540, a, 6);
    return r;
}
// one step of the unit's seed (+0x20), modulo 100
template <class E> int32_t Pct(E& e, uint32_t unit)
{
    const uint64_t v = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x24, (uint32_t)(v >> 32));
    e.wr32(unit + 0x20, (uint32_t)v);
    return (int32_t)((uint32_t)v % 100);
}
}
