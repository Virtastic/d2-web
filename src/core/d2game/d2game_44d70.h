// 1.13c D2Game+0x44D70 (druid melee skill start: to-hit, up to two calc evals, then the combat allocation), transcribed from
// the 1.13c disassembly. Convention: ECX = game, EDX = unit, stack = (skill id, skill level), ret 8. Every callee is one
// E::h(rva, eax, ecx, edx, ebx, esi, stack words...) call as retail passes it; the damage struct is the 0x70-byte stack local
// that EDI addresses for 0xDDDE0 (logged by edi9).
#pragma once
#include <cstdint>

namespace r44d70 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t id, uint32_t level)
{
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)id * 0x23C + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const uint32_t sk = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sk) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    uint32_t dmg[0x1C] = {};
    const uint32_t hit = e.h(0xACD6, 0, 0, 0, 0, 0, unit, (uint32_t)id, level);
    dmg[1] = e.h(0xDE5A0, tgt, 0, 0, 0, 0, game, hit, 0) & 0xFFFF;
    if (dmg[1] & 1) {
        if (e.rd8(rec + 0x1DC)) {
            const int32_t r = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x144), (uint32_t)id, level);
            dmg[0x1A] = (uint32_t)r;
            if (r > 0) dmg[0x19] = (dmg[0x19] & ~0xFF00u) | ((uint32_t)e.rd8(rec + 0x1DC) << 8);
        }
        dmg[3] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)id, level);
    }
    uint32_t flag = e.rd8(rec + 0x1A5);
    if (!flag) flag = 0x80;
    e.h(0xDDDE0, unit, 0, 0, 0, 0, game, tgt);   // ECX (flag byte) has residue in its upper bits: not logged
    e.edi9(0xDDDE0, dmg);
    e.h(0xAC94, 0, 0, 0, 0, 0, sk, (dmg[1] & 1) ? 1u : 0u);
    return 1;
}
}
