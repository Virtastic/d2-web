// 1.13c D2Game+0xD170 (a player's pet skill: a missile from the player's owner unit to a picked target, which gets state 0x76):
// ecx = pGame, edx = pUnit, stack = (skill id, skill level), ret 8. Written from the 1.13c disassembly as a rule over E
// (see d2game_c310.h). Rule: a null or non-player (type != 1) unit returns 0; a skill id outside the table or a null record
// returns 0; the record's server missile (+0x48, signed) must lie in 1..dt+0xB6C exclusive. The owner (0x21260, ecx = unit;
// plain ret) and the target (0xE0BD0, ecx = game; plain ret) must exist and the target must not have state 0x76 (0xA68E);
// then 0xA3CA(target, 0x76, 1), 0xA4DE(target), a 0x6F930 request (ecx = game, edx = 23-dword block: [1] = owner,
// [2] = target, [4] = missile, [11] = id, [12] = level) and 0x7A190 (esi = unit, edi = target, stack = id, 1, 0, 0, 0;
// ret 0x14). Result 1.
#pragma once
#include <cstdint>

namespace d170 {
template <class E> uint32_t PetMissile(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, uint32_t level)
{
    if (!unit || e.rd32(unit) != 1) return 0;
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t m = e.rd16s(rec + 0x48);
    if (m <= 0 || m >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    const uint32_t owner = e.h(0x21260, 0, unit);
    if (!owner) return 0;
    const uint32_t tgt = e.h(0xE0BD0, 0, game);
    if (!tgt) return 0;
    if (e.h(0xA68E, 0, 0, 0, 0, 0, tgt, 0x76)) return 0;
    e.h(0xA3CA, 0, 0, 0, 0, 0, tgt, 0x76, 1);
    e.h(0xA4DE, 0, 0, 0, 0, 0, tgt);
    uint32_t q[23] = {};
    q[1] = owner; q[2] = tgt; q[4] = (uint32_t)m; q[11] = (uint32_t)skillId; q[12] = level;
    e.h(0x6F930, 0, game, e.stk(q));
    e.h(0x7A190, 0, 0, 0, 0, unit, (uint32_t)skillId, 1, 0, 0);
    e.edi9(0x7A190, (const void*)(uintptr_t)tgt);
    return 1;
}
}
