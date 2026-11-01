// 1.13c D2Game+0x451B0 (skill-start tail on an already-fetched record: to-hit, damage struct, notification), transcribed from the
// 1.13c disassembly. Convention: EAX = skill id, EBX = unit, stack = (game, target, value, record pointer, level), ret 0x14; the value
// is raised to at least 10 before it reaches 0x9E9B0. Every callee is one E::h(rva, eax, ecx, edx, ebx, esi, stack words...) call as
// retail passes it; the damage struct is the 0x70-byte stack local (0x9E9B0 takes it in ESI, 0x9F210 on the stack, 0xDDDE0 in EDI,
// logged by edi9). The result is whatever 0xDED30 returns.
#pragma once
#include <cstdint>

namespace r451b0 {
template <class E> uint32_t Do(E& e, int32_t id, uint32_t unit, uint32_t game, uint32_t tgt, int32_t val, uint32_t rec, uint32_t level)
{
    if (val < 10) val = 10;
    uint32_t dmg[0x1C] = {};
    const uint32_t pd = e.stk(dmg);
    uint32_t w = (uint32_t)e.rd16s(rec + 0x12E) & 0xFFFF;
    if (!(w & 1)) {
        const uint32_t hit = e.h(0xACD6, 0, 0, 0, 0, 0, unit, (uint32_t)id, level);
        w = e.h(0xDE5A0, tgt, 0, 0, 0, 0, game, hit, 0) & 0xFFFF;
        if (w & 1) w |= (uint32_t)e.rd16s(rec + 0x12E) & 0xFFFF;
    }
    dmg[1] = w;
    if (dmg[1] & 1) {
        dmg[0] |= e.rd32(rec + 0x130);
        if (e.rd32(rec + 0x134)) dmg[0x18] = e.rd32(rec + 0x134);
        dmg[3] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)id, level);
        e.h(0x9F210, (uint32_t)id, 0, 0, 0, 0, pd, level);
        e.edi9(0x9F210, (const void*)(uintptr_t)unit);
    }
    e.h(0x9E9B0, (uint32_t)id, 0, (uint32_t)val, 0, pd);
    e.h(0xDDDE0, unit, 0x80, 0, 0, 0, game, tgt);
    e.edi9(0xDDDE0, dmg);
    return e.h(0xDED30, 0, 0, 0, 0, 0, game, unit, tgt);
}
}
