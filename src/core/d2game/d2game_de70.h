// 1.13c D2Game+0xDE70 (a skill missile cast at the unit's target, with an overlay and an animation mode): ecx = pGame, edx = pUnit,
// stack = (skill id, skill level), ret 8. Written from the 1.13c disassembly as a rule over E (see d2game_c310.h).
// Rule: a skill id outside the table, a null record or a server missile (rec+0x48, signed) outside 1..dt+0xB6C returns 0; so does a
// null missile row (dt+0xB64, stride 0x1A4) or a null 0xAABA(unit). A target 0xE0BD0 (ecx = game; plain ret) of a unit with a
// nonzero +0x2C gets 0xAB2C(word unit+0x2C, 0x1100(target), 0x1130(target)) and 0xACD0(unit, target). A 23-dword 0x6F930 request (ecx = game,
// edx = block: [0] = 0x25, [1] = unit, [4] = missile, [5], [6] = the unit's 0x1100 / 0x1130, [7], [8] = 0xAAC6 / 0xAAD8 of
// unit+0x2C, [10] = row byte 0x9A + row byte 0x9B * level / 8, [11] = id, [12] = level); a missile gets n = 0xAC9A(unit, rec+0x13C,
// id, level) + row+0x3C through 0xAB14(mis+0x2C, n), 0xAD42(mis, n), 0xAC7C(mis, n). Then 0xB3270(rec). If game+0xA8 is below
// 0xAD12(X) and the unit has state 0xC (0xA68E): each node of the unit's +0xDC list whose first byte is 1 goes to 0x8E6F0
// (eax = node, edi = game; plain ret) and 0x8EA80 (ecx = game, edx = unit, stack = 0, game+0xA8 + 3, 0, 0, 0; ret 0x14).
// Otherwise 0xDE20 runs (inline here): the nodes whose first byte is 0 go to 0x8E6F0, then 0x8EA80(.., 2, game+0xA8 + 0xD, 0, 0, 0).
// Result 1.
#pragma once
#include <cstdint>

namespace de70 {
template <class E> uint32_t TargetMissile(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skillId, int32_t level)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skillId * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t m = e.rd16s(rec + 0x48);
    if (m <= 0 || m >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    const uint32_t row = (uint32_t)m * 0x1A4u + e.rd32(dt + 0xB64);
    if (!row) return 0;
    const uint32_t x = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!x) return 0;
    const uint32_t tgt = e.h(0xE0BD0, 0, game);
    if (tgt && e.rd32(unit + 0x2C)) {
        const uint32_t ty = e.h(0x1130, 0, tgt);
        const uint32_t tx = e.h(0x1100, 0, tgt);
        e.h(0xAB2C, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C), tx, ty);   // the path word of the unit, not the 0xAABA result
        e.h(0xACD0, 0, 0, 0, 0, 0, unit, tgt);
    }
    uint32_t q[23] = {};
    q[0] = 0x25; q[1] = unit;
    q[5] = e.h(0x1100, 0, unit);
    q[6] = e.h(0x1130, 0, unit);
    q[11] = (uint32_t)skillId;
    q[12] = (uint32_t)level; q[4] = (uint32_t)m;
    q[10] = (uint32_t)((int32_t)((uint32_t)e.rd8(row + 0x9B) * (uint32_t)level) / 8 + (int32_t)e.rd8(row + 0x9A));
    q[7] = e.h(0xAAC6, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    q[8] = e.h(0xAAD8, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    const uint32_t mis = e.h(0x6F930, 0, game, e.stk(q));
    if (mis) {
        const uint32_t n = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x13C), (uint32_t)skillId, (uint32_t)level) + e.rd32(row + 0x3C);
        e.h(0xAB14, 0, 0, 0, 0, 0, e.rd32(mis + 0x2C), n);
        e.h(0xAD42, 0, 0, 0, 0, 0, mis, n);
        e.h(0xAC7C, 0, 0, 0, 0, 0, mis, n);
    }
    e.h(0xB3270, 0, 0, 0, 0, 0, rec);
    const uint32_t lim = e.h(0xAD12, 0, 0, 0, 0, 0, x);
    const bool over = (int32_t)e.rd32(game + 0xA8) < (int32_t)lim && e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC);
    const uint8_t want = over ? 1 : 0;
    for (uint32_t nd = e.rd32(unit + 0xDC); nd;) {
        const uint32_t nx = e.rd32(nd + 0x24);
        if (e.rd8(nd) == want) e.h(0x8E6F0, nd);
        nd = nx;
    }
    e.h(0x8EA80, 0, game, unit, 0, 0, over ? 0u : 2u, e.rd32(game + 0xA8) + (over ? 3u : 0xDu), 0, 0, 0);
    return 1;
}
}
