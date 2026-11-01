// 1.13c D2Game+0xD290 (a skill missile launched from the caster toward its target, its velocity the target's offset):
// ecx = pGame, edx = pUnit, stack = (skill id, skill level), ret 8. Written from the 1.13c disassembly as a rule over E
// (see d2game_c310.h). Rule: a skill id outside the table or a null record returns 0; the record's server missile (+0x48,
// signed) must lie in 1..dt+0xB6C exclusive. A unit gets flags (+0xC4) |= 0x40. 0x9FA30 (eax = unit, ebx = &x, stack =
// &y; ret 4) gives the target's position (none returns 0); dx = x - 0x1100(unit), dy = y - 0x1130(unit) (ecx = unit, plain
// ret). 0xA28F0 (ecx = game, edx = missile, stack = unit, id, level, 0 x5; ret 0x20) makes the missile (none returns 0);
// 0xA50E(missile, -dy) and 0xAF5E(missile, dx) set it going. Result 1.
#pragma once
#include <cstdint>

namespace d290 {
template <class E> uint32_t AimedMissile(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, uint32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t m = e.rd16s(rec + 0x48);
    if (m <= 0 || m >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    uint32_t pos[2] = { 0, 0 };
    if (!e.h(0x9FA30, unit, 0, 0, e.stk(&pos[0]), 0, e.stk(&pos[1]))) return 0;
    const uint32_t dx = pos[0] - e.h(0x1100, 0, unit);
    const uint32_t dy = pos[1] - e.h(0x1130, 0, unit);
    const uint32_t mis = e.h(0xA28F0, 0, game, (uint32_t)m, 0, 0, unit, (uint32_t)skillId, level, 0);
    if (!mis) return 0;
    e.h(0xA50E, 0, 0, 0, 0, 0, mis, 0u - dy);
    e.h(0xAF5E, 0, 0, 0, 0, 0, mis, dx);
    return 1;
}
}
