// stand-alone 32-bit layout twin of D2Common 0x52210 (the proven row runs SKILLS_CheckSkillDelay) -- 1 unless a player's state 0x79 refuses the skill (EAX = unit, stack skill, ret 4).
// Written from the disassembly (the decompile hides EAX). Offsets (1.13c, 32-bit): unit +0 type; skill +0 skills.txt record,
// +0x28 base level, +0x34 charge owner (-1 = learned, the bonus applies); record +0 (s16) id, +0x190 calc index;
// data tables +0xC4 states count.
#include <cstdint>
#include <cstring>
#include "d2common_52210.h"

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
inline int16_t rd16(uint32_t a) { int16_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 2); return v; }
}

extern "C" uint32_t D2Common_52210_StateSkillGate(uint32_t unit, uint32_t skill, const D2Common52210Env* e)
{
    if (!unit || rd32(unit) != 0) return 1;
    if ((int32_t)rd32(e->dataTables + 0xC4) <= 0x79) return 1;
    if (!e->hasState(unit, 0x79)) return 1;
    if (!skill) return 1;
    const uint32_t txt = rd32(skill);
    if (!txt) return 1;
    int32_t lvl = (int32_t)rd32(skill + 0x28);
    if ((int32_t)rd32(skill + 0x34) == -1) lvl += e->bonus(skill, unit);
    if (lvl < 0) lvl = 0;
    const int32_t cap = (int32_t)rd32(e->maxLevel);
    if (lvl > cap) lvl = cap;
    return e->formula(unit, rd32(txt + 0x190), rd16(txt), lvl) <= 0 ? 1u : 0u;
}
