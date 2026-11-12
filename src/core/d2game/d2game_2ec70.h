// 1.13c D2Game+0x2EC70: object init callback "hell brazier" (table entry). ecx = the init block ([0] game, [4] the
// object). The game's object seed (pointer at game +0x10F0, two words) advances one step: low * 0x6AC690C5 + high,
// low word back to +0, carry word to +4. An odd new low word puts the object in mode 1 (0xA4A2: object, 1) and
// that call's result is the result; an even one gives 0.
#pragma once
#include <cstdint>

namespace g2ec70 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t seed = e.rd32(e.rd32(op) + 0x10F0);
    const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5u + e.rd32(seed + 4);
    e.wr32(seed, (uint32_t)v);
    e.wr32(seed + 4, (uint32_t)(v >> 32));
    if (!(v & 1)) return 0;
    return e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1);
}
}
