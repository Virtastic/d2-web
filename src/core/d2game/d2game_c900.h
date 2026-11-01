// 1.13c D2Game+0xC900 (SKILLS_SrvDo134_SiegeBeastStomp): ecx = pGame, edx = pUnit, stack = (skill id, skill level), ret 8.
// Written from the 1.13c disassembly as a rule over an environment E (see d2game_c310.h): every callee is one E::h call.
// Rule: a skill id outside the table, a null record, or a record whose srv-missile word (+0x48) is not in 1..count
// (dt+0xB6C, exclusive) returns 0. Otherwise unit flags (+0xC4) |= 0x40, a 28-dword damage block is zeroed, word 0 :=
// 1 | record+0x130, the low half of word 1 |= record+0x12E, word 24 := record+0x134 when that is set; 0x9F1A0 and
// 0x9F210 (eax = id, ebx = unit, stack = &block, level) fill it; the radius is calc record+0x64 evaluated by 0xAC9A
// (unit, calc, id, level); with a unit, 0xA0C70 (eax = game, stack = unit, 0, 0, radius, 0x8583, callback, &block, 0)
// applies it. Result 1.
#pragma once
#include <cstdint>

namespace c900 {
template <class E> uint32_t SiegeBeastStomp(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, int32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t m = e.rd16s(rec + 0x48);
    if (m <= 0 || m >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    uint32_t blk[28] = {};
    blk[0] |= 1;
    blk[1] |= e.rd32(rec + 0x12C) >> 16;   // the word at +0x12E into the low half of word 1
    blk[0] |= e.rd32(rec + 0x130);
    if (e.rd32(rec + 0x134)) blk[24] = e.rd32(rec + 0x134);
    e.h(0x9F1A0, (uint32_t)skillId, 0, 0, unit, 0, e.stk(blk), (uint32_t)level);
    e.h(0x9F210, (uint32_t)skillId, 0, 0, unit, 0, e.stk(blk), (uint32_t)level);
    const uint32_t r = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x64), (uint32_t)skillId, (uint32_t)level);
    if (unit) {
        e.h(0xA0C70, game, 0, 0, 0, 0, unit, 0, 0, r);
        e.blk(0xA0C70, blk, 28);
    }
    return 1;
}
}
