// 1.13c D2Game+0x44550 (SKILLS_SrvSt11_Inferno_ArcticBlast): ecx = pGame, edx = pUnit, stack = (skill id, skill level), ret 8.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape). Rule: skill id outside [0, skills count)
// or a null record -> 0. Unless the unit has state 0xC (0xA68E thunk, stack unit, 0xC), its stat 8 (0xA42A thunk, stack
// unit, 8, 0) below startmana (+0x184, signed word) << 8 -> 0. srvmissilea (+0x48, signed word) outside [0, missiles count)
// -> 0; otherwise the result of 0x443F0 (eax = unit, ebx = game, stack = skill id, level, missile; ret 0xC).
#pragma once
#include <cstdint>

namespace d2g44550 {
template <class E> uint32_t SrvSt11(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, uint32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC) == 0) {
        const int32_t mana = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 8, 0);
        if (mana < e.rd16s(rec + 0x184) * 0x100) return 0;
    }
    const int32_t missile = e.rd16s(rec + 0x48);
    if (missile < 0 || missile >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    return e.h(0x443F0, unit, 0, 0, game, 0, (uint32_t)skillId, level, (uint32_t)missile);
}
}
