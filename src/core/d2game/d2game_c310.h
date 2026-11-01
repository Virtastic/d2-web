// 1.13c D2Game+0xC310 (SKILLS_SrvDo128_CryHelp): ecx = pGame, edx = pUnit, stack = (skill id, skill level), ret 8.
// Written from the 1.13c disassembly as a rule over an environment E (see skill_helpers_a19a0.h): every callee that
// is not a field read is one E::h(rva, ...) call. Rule: a skill id outside the table or with a null record -> 0;
// refresh the unit's target (0xE07A0) and look it up (0xA598 on unit+0x2C); no target, the unit itself or a
// non-monster caster -> 0. Otherwise the timeout is calc +0x138 evaluated with (unit, calc, id, level), clamped to at
// least 1; a six-word command block (words 2..5 = 1, target type, target id, game frame + timeout; words 0 and 1
// are never written) goes to 0x212F0 (eax = unit, stack = game, &block); a server overlay in 1..overlay count
// (exclusive) is set on the target (0xA83E); result 1.
#pragma once
#include <cstdint>

namespace c310 {
template <class E> uint32_t CryHelp(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, int32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt || e.rd32(unit) != 1) return 0;
    uint32_t blk[6] = {};
    blk[2] = 1;
    blk[3] = e.rd32(tgt);
    blk[4] = e.rd32(tgt + 0xC);
    int32_t t = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)skillId, (uint32_t)level);
    if (t < 1) t = 1;
    blk[5] = e.rd32(game + 0xA8) + (uint32_t)t;
    e.h(0x212F0, unit, 0, 0, 0, 0, game, e.stk(blk));
    e.blk(0x212F0, blk, 6, 0xFC);
    const int32_t ov = e.rd16s(rec + 0x4E);
    if (ov > 0 && ov < (int32_t)e.rd32(dt + 0xBC0)) e.h(0xA83E, 0, 0, 0, 0, 0, tgt, (uint32_t)ov, 0);
    return 1;
}
}
