// 1.13c D2Game+0xCDC0 (corpse-raise skill: a monster of the corpse's class rises at the corpse): ecx = pGame, edx = pUnit,
// stack = (skill id, skill level: unused), ret 8. Written from the 1.13c disassembly as a rule over E (see d2game_c310.h).
// Rule: a skill id outside the table or a null record returns 0. The unit's corpse target (0xAABA, #10511) must exist;
// from it 0xAD12, 0xACB2 (x), 0xAD3C (y) and 0xAB44 are read; the unit's room (0xA478) and the room holding (x, y)
// (0x1280: eax = room, ebx = x, edi = y, plain ret) must exist. 0xEF650 (ecx = AD12 value, edx = AB44 value, stack =
// game, room, x, y, -1, 0; ret 0x18) spawns the monster, retried once with mode 4 in place of -1; none returns 0. The
// monster's flags (+0xC4) |= 0x4020000, its owner is set (0xAC76: monster, unit) and the record's overlay (+0xE6, signed,
// 0..overlay count exclusive) goes on it (0xA83E: monster, overlay, 0). Result 1.
#pragma once
#include <cstdint>

namespace cdc0 {
template <class E> uint32_t RaiseCorpse(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const uint32_t corpse = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!corpse) return 0;
    const uint32_t cls = e.h(0xAD12, 0, 0, 0, 0, 0, corpse);
    const uint32_t x = e.h(0xACB2, 0, 0, 0, 0, 0, corpse);
    const uint32_t y = e.h(0xAD3C, 0, 0, 0, 0, 0, corpse);
    const uint32_t w = e.h(0xAB44, 0, 0, 0, 0, 0, corpse);
    const uint32_t room0 = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t room = e.h(0x1280, room0, 0, 0, x);
    if (!room) return 0;
    uint32_t mon = e.h(0xEF650, 0, cls, w, 0, 0, game, room, x, y);
    if (!mon) mon = e.h(0xEF650, 0, cls, w, 0, 0, game, room, x, y);
    if (!mon) return 0;
    e.wr32(mon + 0xC4, e.rd32(mon + 0xC4) | 0x4020000);
    e.h(0xAC76, 0, 0, 0, 0, 0, mon, unit);
    const int32_t ov = e.rd16s(rec + 0xE6);
    if (ov >= 0 && ov < (int32_t)e.rd32(dt + 0xBC0)) e.h(0xA83E, 0, 0, 0, 0, 0, mon, (uint32_t)ov, 0);
    return 1;
}
}
