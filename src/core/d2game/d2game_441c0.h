// 1.13c D2Game+0x441C0 SKILLS_DoInferno, transcribed from the 1.13c disassembly. Convention: EAX = unit, stack = (game, skill id,
// skill level, missile id), ret 0x10; returns 1 when handled, 0 on a rejected argument. sgptDataTables is read as dt (+0xB6C missile count,
// +0xB98 skills rows of 0x23C bytes, +0xBA0 skill count). Callees are hooks on an env (HookEnv logs them as the capture does):
// 0x9FA30 (EAX unit, ECX game, EBX = &x, stack &y), 0xAABA (unit gives the used skill), 0xAD12 (skill gives param 1),
// 0xAC9A (unit, calc, skill id, level give the range), 0x6F930 (ECX game, EDX = 23-dword request), 0xAC94 (skill, 1),
// 0xA68E (unit, 0xC), 0x8E6F0 (EAX node), 0x8EA80 (ECX game, EDX unit, five stack words),
// 0x43CA0 (EAX game, EBX unit), 0xE2480 (ESI unit, stack game, 1).
#pragma once
#include <cstdint>

namespace r441c0 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t unit, uint32_t game, int32_t skillId, int32_t level, int32_t missile)
{
    if (skillId < 0 || skillId >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = e.rd32(dt + 0xB98) + (uint32_t)skillId * 0x23C;
    if (rec == 0) return 0;
    if (missile < 0 || missile >= (int32_t)e.rd32(dt + 0xB6C)) return 0;

    uint32_t x = 0, y = 0;
    if (!e.h(0x9FA30, unit, game, 0, e.stk(&x), 0, e.stk(&y))) return 0;
    const uint32_t skill = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!skill) return 0;

    if (e.h(0xAD12, 0, 0, 0, 0, 0, skill)) {
        int32_t range = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)skillId, (uint32_t)level);
        if (range < 1) range = 1;
        uint32_t b[23] = {};
        b[0] = 0x8020;
        b[1] = unit;
        b[2] = unit;
        b[4] = (uint32_t)missile;
        b[7] = x;
        b[8] = y;
        b[11] = (uint32_t)skillId;
        b[12] = (uint32_t)level;
        b[19] = (uint32_t)range;
        e.h(0x6F930, 0, game, e.stk(b));
        e.blk(0x6F930, b, 23);
    }
    e.h(0xAC94, 0, 0, 0, 0, 0, skill, 1);

    if (unit && e.rd32(unit) == 1) {
        e.wr32(unit + 0x44, 0xB00);
        const int32_t p1 = (int32_t)e.h(0xAD12, 0, 0, 0, 0, 0, skill);
        const int32_t frame = (int32_t)e.rd32(game + 0xA8);
        if (frame < p1 && e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC)) {
            for (uint32_t n = e.rd32(unit + 0xDC); n;) {
                const uint32_t next = e.rd32(n + 0x24);
                if (e.rd8(n) == 1) e.h(0x8E6F0, n);
                n = next;
            }
            e.h(0x8EA80, 0, game, unit, 0, 0, 0, (uint32_t)(frame + 2), 0, 4);
            return 1;
        }
        e.h(0x43CA0, game, 0, 0, unit);
        return 1;
    }
    e.h(0xE2480, 0, 0, 0, 0, unit, game, 1);
    return 1;
}
}
