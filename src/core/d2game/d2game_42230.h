// 1.13c D2Game+0x42230: skill start callback of the thunder storm skill (table entry). ecx unread, edx = the unit,
// two stack words (ret 8): the skill id, one unread. The skill id must be inside the skill table (data tables
// +0xBA0 records at +0xB98, 0x23C bytes each) and the unit must own the skill (0xA3F4: ecx = unit, edx = skill
// id), else 0. The skill's first parameter is set to -1 (0xAC94: skill, -1) and its second to 1 (0xACDC: skill,
// 1); the result is 1.
#pragma once
#include <cstdint>

namespace g42230 {
template <class E> uint32_t Start(E& e, uint32_t dt, uint32_t unit, uint32_t skillId)
{
    if ((int32_t)skillId < 0 || (int32_t)skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    if (!(skillId * 0x23C + e.rd32(dt + 0xB98))) return 0;
    const uint32_t sk = e.h(0xA3F4, 0, unit, skillId);
    if (!sk) return 0;
    e.h(0xAC94, 0, 0, 0, 0, 0, sk, 0xFFFFFFFFu);
    e.h(0xACDC, 0, 0, 0, 0, 0, sk, 1);
    return 1;
}
}
