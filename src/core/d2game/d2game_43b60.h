// 1.13c D2Game+0x43B60 (SKILLS_SrvDo017_ChargedBolt_BoltSentry): ecx = pGame, edx = pUnit, stack = (skill id, skill level), ret 8.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape; E::blk logs the request block, E::modAddr(rva) is a
// module address as the log records it). Rule: a unit gets +0xC4 |= 0x40. Skill id outside [0, skills count) or a null record -> 0.
// m = 0x91340 (eax = skill id, stack unit; ret 4); m outside [0, missiles count) -> 0. n = import 0xAC9A(unit, record +0x138, skill, level)
// (ret 0x10). A 23-dword request is zeroed: [0] 0x21, [1] unit, [4] m, [5] 0x1100(ecx = unit), [6] 0x1130(ecx = unit) (plain ret), [0xB]
// skill, [0xC] level, [0x15] module 0x41F50. For i in [0, n): [0x16] := i; 0x9FA30 (eax = unit, ecx = game, ebx = &req[7], stack &req[8];
// ret 4) nonzero with req[7] and req[8] nonzero -> 0x6F930 (ecx = game, edx = &req; plain ret). Result 1.
#pragma once
#include <cstdint>

namespace d2g43b60 {
template <class E> uint32_t SrvDo017(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t skill, uint32_t level)
{
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    if (skill < 0 || skill >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)skill * 0x23Cu + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    const int32_t m = (int32_t)e.h(0x91340, (uint32_t)skill, 0, 0, 0, 0, unit);
    if (m < 0 || m >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    const int32_t n = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)skill, level);
    uint32_t req[23] = {};
    req[0] = 0x21;
    req[1] = unit;
    req[5] = e.h(0x1100, 0, unit);
    req[6] = e.h(0x1130, 0, unit);
    req[0xB] = (uint32_t)skill;
    req[0xC] = level;
    req[4] = (uint32_t)m;
    req[0x15] = e.modAddr(0x41F50);
    for (int32_t i = 0; i < n; ++i) {
        req[0x16] = (uint32_t)i;
        const uint32_t ok = e.h(0x9FA30, unit, game, 0, e.stk(&req[7]), 0, e.stk(&req[8]));
        if (ok && req[7] && req[8]) { e.h(0x6F930, 0, game, e.stk(req)); e.blk(0x6F930, req, 23); }
    }
    return 1;
}
}
