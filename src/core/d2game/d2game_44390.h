// 1.13c D2Game+0x44390 (SKILLS_SrvDo019_Inferno_ArcticBlast): ecx = pGame, edx = pUnit, stack = (skill id, skill level), ret 8.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape: rd32/rd16s field reads, h(rva, ...) per callee).
// Rule: skill id outside [0, skills count) or a null record -> 0; the record's srvmissilea (+0x48, signed word) outside
// [0, missiles count) -> 0; otherwise the result of 0x441C0 (eax = unit, stack = game, skill id, level, missile; ret 0x10).
#pragma once
#include <cstdint>

namespace d2g44390 {
template <class E> uint32_t SrvDo019(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, uint32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t missile = e.rd16s(rec + 0x48);
    if (missile < 0 || missile >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    return e.h(0x441C0, unit, 0, 0, 0, 0, game, (uint32_t)skillId, level, (uint32_t)missile);
}
}
