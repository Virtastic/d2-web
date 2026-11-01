// 1.13c D2Game+0xD850 (SKILLS_SrvDo131_OverseerWhip): eax = skill id, ecx = pUnit, stack = (pGame, pTarget, skill level), ret 0xC.
// Written from the 1.13c disassembly as a rule over E (see d2game_c310.h). Rule: a skill id outside the table or a null record
// returns (eax = the id). 0x9EE50 (eax = ecx = 0, edx = &the pGame stack slot as an out-dword, stack = target, 0, id, level;
// ret 0x10) gives a class; a negative class, a null unit or a non-player unit returns. 0xACBE(unit+4, 0, &the level slot) fills
// that slot; 0x1D30 (ecx = class, ebx = that slot; plain ret) gives v; 0xAFC50 (eax = unit, stack = game, v, the pGame slot; ret 0xC);
// 0x9D230 (ecx = game, edx = unit, stack = [[unit+0x14]+0x28] for a player, 0xF; ret 8); 0x75070 (esi = a 7-dword stack block,
// stack = the pGame slot; ret 4) and 0x75E10(game, block, 1); 0xA3CA(unit, 0x8E, 1). An item 0xA6A0([game+0x1C], 0, 0, type, id)
// of the target (type 6, id -1 without one) gets 0xA60A(unit, item, 1), 0xA5B6(item, 0x8E), 0xA6E2(item, 0x163, v, 0). Last,
// rec+0xE4 (signed word) in 1..0x2A runs 0x232B0 (ebx = it, esi = game, stack = unit, 0; ret 8), whose eax is the result;
// otherwise eax keeps the record's upper half under that word (callers drop the result).
#pragma once
#include <cstdint>

namespace d850 {
template <class E> uint32_t OverseerWhip(E& e, uint32_t dt, int32_t skillId, uint32_t unit, uint32_t game, uint32_t target, uint32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return (uint32_t)skillId;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return (uint32_t)skillId;
    uint32_t slot0 = game, slot2 = level;
    const uint32_t cls = e.h(0x9EE50, 0, 0, e.stk(&slot0), 0, 0, target, 0, (uint32_t)skillId, level);
    if ((int32_t)cls < 0 || !unit) return cls;
    if (e.rd32(unit) != 1) return e.rd32(unit);
    e.h(0xACBE, 0, 0, 0, 0, 0, e.rd32(unit + 4), 0, e.stk(&slot2));
    const uint32_t v = e.h(0x1D30, 0, cls, 0, slot2);
    e.h(0xAFC50, unit, 0, 0, 0, 0, game, v, slot0);
    const uint32_t pd = e.rd32(unit + 0x14);
    e.h(0x9D230, 0, game, unit, 0, 0, pd ? e.rd32(pd + 0x28) : 0, 0xF);
    uint32_t blk[7] = {};
    e.h(0x75070, 0, 0, 0, 0, e.stk(blk), slot0);
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
    e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0x8E, 1);
    const uint32_t type = target ? e.rd32(target) : 6u, gid = target ? e.rd32(target + 0xC) : 0xFFFFFFFFu;
    const uint32_t item = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 0, 0, type, gid);
    if (item) {
        e.h(0xA60A, 0, 0, 0, 0, 0, unit, item, 1);
        e.h(0xA5B6, 0, 0, 0, 0, 0, item, 0x8E);
        e.h(0xA6E2, 0, 0, 0, 0, 0, item, 0x163, v, 0);
    }
    const int16_t w = e.rd16s(rec + 0xE4);
    if (w > 0 && w < 0x2B) return e.h(0x232B0, 0, 0, 0, (uint32_t)(int32_t)w, game, unit, 0);
    return (rec & 0xFFFF0000u) | (uint16_t)w;
}
}
