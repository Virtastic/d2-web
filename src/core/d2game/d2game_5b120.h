// Act V quest arm step (shared by three Act V quest variants): ECX = game, EDX = unit, one unused stack word.
// The unit's quest record comes from the common library; its per-difficulty flag block (record+0x10+4*difficulty, difficulty
// at game+0x6D) goes to the quest-state test with the quest id and 0xD. When that test passes and the unit is an armed player
// (type 0, +0x80 set), the party lookup (EAX = unit) names its party; a party other than -1 runs the quest callback over the
// party members (EAX = party, ESI = game, EBX = 0, the callback as its one stack word). The result is 0.
#pragma once
#include <cstdint>

namespace d2gArmQuestCb {
template <class E> uint32_t Arm(E& e, uint32_t game, uint32_t unit, uint32_t quest, uint32_t callback)
{
    const uint32_t rec = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t flags = e.rd32(rec + 0x10 + 4 * e.rd8(game + 0x6D));
    if (!e.h(0xA568, 0, 0, 0, 0, 0, flags, quest, 0xD)) return 0;
    if (!unit || !e.rd32(unit + 0x80) || e.rd32(unit) != 0) return 0;
    const uint32_t party = e.h(0x9BEB0, unit);
    if ((uint16_t)party == 0xFFFF) return 0;
    e.h(0x9C160, party, 0, 0, 0, game, callback);
    return 0;
}
// 1.13c D2Game+0x5B120
template <class E> uint32_t Run5B120(E& e, uint32_t game, uint32_t unit, uint32_t mod) { return Arm(e, game, unit, 0x25, mod + 0x580A0); }
}
