// 1.13c D2Game+0x441C0 (SKILLS_DoInferno): eax = pUnit, stack = (game, skill id, skill level, missile), ret 0x10.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape; E::stk is a local's address, E::blk logs a block).
// Rule: skill id outside [0, skills count), a null record or the missile outside [0, missiles count) -> 0. 0x9FA30 (eax = unit, ecx = game,
// ebx = the skill-id argument slot, stack &local; ret 4) zero -> 0. r = import 0xAABA(unit); null -> 0. Import 0xAD12(r) nonzero: n =
// import 0xAC9A(unit, record +0x138, skill, level), at least 1; a zeroed 23-dword request [0] 0x8020, [1] [2] unit, [4] missile, [7] the
// argument slot, [8] the local, [0xB] skill, [0xC] level, [0x13] n goes to 0x6F930 (ecx = game, edx = &request). Import 0xAC94(r, 1).
// A player (unit type 1): +0x44 := 0xB00; game +0xA8 below import 0xAD12(r) (signed) and import 0xA68E(unit, 0xC) nonzero: every node
// on unit +0xDC (next +0x24) whose first byte is 1 goes to 0x8E6F0 (eax = node, ebx = next; plain ret), then 0x8EA80 (ecx = game,
// edx = unit, stack 0, game +0xA8 + 2, 0, 4, 0; ret 0x14); else 0x43CA0 (eax = game, ebx = unit; plain ret). Any other unit: 0xE2480
// (stack game, 1; ret 8). Result 1.
#pragma once
#include <cstdint>

namespace d2g441c0 {
template <class E> uint32_t DoInferno(E& e, uint32_t dt, uint32_t unit, uint32_t game, int32_t skill, uint32_t level, int32_t missile)
{
    if (skill < 0 || skill >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skill * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    if (missile < 0 || missile >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    uint32_t slot = (uint32_t)skill, loc = 0;
    if (!e.h(0x9FA30, unit, game, 0, e.stk(&slot), 0, e.stk(&loc))) return 0;
    const uint32_t room = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!room) return 0;
    if (e.h(0xAD12, 0, 0, 0, 0, 0, room)) {
        int32_t n = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)skill, level);
        if (n < 1) n = 1;
        uint32_t req[23] = {};
        req[0] = 0x8020;
        req[1] = req[2] = unit;
        req[4] = (uint32_t)missile;
        req[7] = slot;
        req[8] = loc;
        req[0xB] = (uint32_t)skill;
        req[0xC] = level;
        req[0x13] = (uint32_t)n;
        e.h(0x6F930, 0, game, e.stk(req));
        e.blk(0x6F930, req, 23);
    }
    e.h(0xAC94, 0, 0, 0, 0, 0, room, 1);
    if (unit && e.rd32(unit) == 1) {
        e.wr32(unit + 0x44, 0xB00);
        const int32_t v = (int32_t)e.h(0xAD12, 0, 0, 0, 0, 0, room);
        if ((int32_t)e.rd32(game + 0xA8) < v && e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC)) {
            for (uint32_t nd = e.rd32(unit + 0xDC); nd;) {
                const uint32_t nx = e.rd32(nd + 0x24);
                if (e.rd8(nd) == 1) e.h(0x8E6F0, nd, 0, 0, nx);
                nd = nx;
            }
            e.h(0x8EA80, 0, game, unit, 0, 0, 0, e.rd32(game + 0xA8) + 2, 0, 4);
            return 1;
        }
        e.h(0x43CA0, game, 0, 0, unit);
        return 1;
    }
    e.h(0xE2480, 0, 0, 0, 0, 0, game, 1);
    return 1;
}
}
