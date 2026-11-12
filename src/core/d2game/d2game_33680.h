// 1.13c D2Game+0x33680: object init callback "fissure" (table entry). ecx = the init block ([0] game, [4] the
// object). The game's object seed (pointer at game +0x10F0, two words) advances one step: low * 0x6AC690C5 + high,
// low word back to +0, carry word to +4. An event of type 8 is queued 25 frames plus (new low word mod 250) ahead
// (0x8EA80: game, object, 8, frame + 25 + roll, 0, 0, 0) and that call's result is the result.
#pragma once
#include <cstdint>

namespace g33680 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t game = e.rd32(op);
    const uint32_t seed = e.rd32(game + 0x10F0);
    const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5u + e.rd32(seed + 4);
    e.wr32(seed + 4, (uint32_t)(v >> 32));
    e.wr32(seed, (uint32_t)v);
    const uint32_t roll = (uint32_t)v % 250u;
    return e.h(0x8EA80, 0, game, e.rd32(op + 4), 0, 0, 8, e.rd32(game + 0xA8) + roll + 0x19, 0, 0, 0);
}
}
