// Shared steps of the 1.13c D2Game object operate callbacks. The operate block: [0] game, [4] the object,
// [8] the player, [0xC] the seed pair, [0x10] the object's class.
#pragma once
#include <cstdint>

namespace gobjop {
// one step of a seed pair (low, high): the 64-bit value low * 0x6AC690C5 + high replaces the pair; the answer
// is the new low word
template <class E> uint32_t Roll(E& e, uint32_t seed)
{
    const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5u + e.rd32(seed + 4);
    e.wr32(seed + 4, (uint32_t)(v >> 32));
    e.wr32(seed, (uint32_t)v);
    return (uint32_t)v;
}
// the object leaves the "can be operated" state: bit 1 of its flag word (+0xC4) is cleared
template <class E> void ClearOperable(E& e, uint32_t obj)
{
    if (obj) e.wr32(obj + 0xC4, e.rd32(obj + 0xC4) & ~2u);
}
// the tail of the chest-like callbacks: unless the class record's byte +0x121 is set the object's collision
// is dropped (0xA550: object), then the sound of the object data's byte +4 (bit 7 off) is queued (0x6DD60)
template <class E> void OpenTail(E& e, uint32_t op, uint32_t rec)
{
    if (!e.rd8(rec + 0x121)) e.h(0xA550, 0, 0, 0, 0, 0, e.rd32(op + 4));
    const uint32_t obj = e.rd32(op + 4);
    e.h(0x6DD60, 0, 0, 0, 0, 0, e.rd8(e.rd32(obj + 0x14) + 4) & 0x7Fu);
}
// the mode-1 timer: 0x8EA80 (ecx = game, edx = object; stack: 1, game frame (+0xA8) + (record +0xDC >> 8) + 1, 0, 0, 0)
template <class E> void ModeTimer(E& e, uint32_t op, uint32_t rec)
{
    const uint32_t game = e.rd32(op);
    e.h(0x8EA80, 0, game, e.rd32(op + 4), 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + e.rd32(game + 0xA8) + 1, 0, 0, 0);
}
}
