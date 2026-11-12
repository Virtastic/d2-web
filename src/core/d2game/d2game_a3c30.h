// 1.13c D2Game+0xA3C30: AI think callback "initialise the raven" (table entry). ecx = game, edx = the monster, one
// stack word (ret 4): the AI tick block ([0] AI control, [0x1C] the monster's stats record). The control's +0x14
// becomes -1; its +0x18 becomes one more than the signed word the record keeps for the game's difficulty (byte at
// game +0x6D; words from record +0x62). The monster's seed (+0x20 low, +0x24 high) advances one step
// (low * 0x6AC690C5 + high) and the new low word's bit 0 goes to control +0x1C and is the result.
#pragma once
#include <cstdint>

namespace ga3c30 {
template <class E> uint32_t Think(E& e, uint32_t game, uint32_t unit, uint32_t tick)
{
    const uint32_t rec = e.rd32(tick + 0x1C);
    const uint32_t ai = e.rd32(tick);
    e.wr32(ai + 0x14, 0xFFFFFFFFu);
    const uint32_t w = e.rd32(rec + e.rd8(game + 0x6D) * 2 + 0x62) & 0xFFFF;
    e.wr32(ai + 0x18, (uint32_t)((int32_t)(int16_t)w + 1));
    const uint64_t v = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)v);
    e.wr32(unit + 0x24, (uint32_t)(v >> 32));
    e.wr32(ai + 0x1C, (uint32_t)v & 1);
    return (uint32_t)v & 1;
}
}
