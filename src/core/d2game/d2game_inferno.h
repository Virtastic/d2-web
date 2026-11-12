// 1.13c D2Game: the body shared by the two monster inferno skill start callbacks (0xBEE0 reads the skill record's
// calc at +0x138, 0xB2ED0 the one at +0x13C). ecx = the game, edx = the unit, two stack words (ret 8): the skill
// id and the skill level. The skill id must be inside the skill table (data tables +0xBA0 records at +0xB98,
// 0x23C bytes each) and the unit must have a current skill (0xAABA: unit), else 0. The duration is the record's
// calc evaluated for the unit (0xAC9A: unit, calc, skill id, level), at least 1; the skill's first parameter is
// set to the game's frame (+0xA8) plus it (0xAC94: skill, frame). When the unit lacks state 12 (0xA68E: unit,
// 0x0C) the state is set (0xA3CA: unit, 0x0C, 1). The result is 1.
#pragma once
#include <cstdint>

template <class E> uint32_t D2GameInfernoStart(E& e, uint32_t dt, uint32_t calcOff, uint32_t game, uint32_t unit, uint32_t skillId, uint32_t level)
{
    if ((int32_t)skillId < 0 || (int32_t)skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = skillId * 0x23C + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sk) return 0;
    int32_t n = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + calcOff), skillId, level);
    if (n < 1) n = 1;
    e.h(0xAC94, 0, 0, 0, 0, 0, sk, e.rd32(game + 0xA8) + (uint32_t)n);
    if (!e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC)) e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0xC, 1);
    return 1;
}
