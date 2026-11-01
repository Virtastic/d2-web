// 1.13c D2Game+0x45000 (skill start with additive damage terms and a trailing 0xDED30 notification), transcribed from the 1.13c
// disassembly. Convention: ECX = game, EDX = unit, stack = (skill id, skill level), ret 8. Every callee is one
// E::h(rva, eax, ecx, edx, ebx, esi, stack words...) call as retail passes it; the damage struct is the 0x70-byte stack local
// (0xA11B0 writes dmg+4 through ESI, 0xDD450 / 0x9F210 take it on the stack, 0xDDDE0 through EDI, logged by edi9). 0x14D0 is the
// percent helper (EAX = value, ECX = 100, EDX = base), a plain register call.
#pragma once
#include <cstdint>

namespace r45000 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t id, uint32_t level)
{
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = (uint32_t)id * 0x23C + e.rd32(dt + 0xB98);
    if (!rec) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    uint32_t dmg[0x1C] = {};
    const uint32_t pd = e.stk(dmg);
    e.h(0xA11B0, 0, level, (uint32_t)id, rec, pd, game, unit, tgt, 0);
    if (!(dmg[1] & 1)) return 1;
    dmg[0] |= 2;
    dmg[0] |= e.rd32(rec + 0x130);
    dmg[1] = (dmg[1] & ~0xFFFFu) | ((dmg[1] | (uint32_t)e.rd16s(rec + 0x12E)) & 0xFFFFu);
    if (e.rd32(rec + 0x134)) dmg[0x18] = e.rd32(rec + 0x134);
    e.h(0xDD450, 0, e.rd8(rec + 0x1A5), 0, 0, 0, game, unit, tgt, pd);
    e.h(0x9F210, (uint32_t)id, 0, 0, 0, 0, pd, level);
    e.edi9(0x9F210, (const void*)(uintptr_t)unit);
    const uint32_t r0 = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), (uint32_t)id, level);
    dmg[2] += e.h(0x14D0, r0, 0x64, dmg[2]);
    const uint32_t r1 = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x13C), (uint32_t)id, level);
    const uint32_t r2 = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x140), (uint32_t)id, level);
    dmg[0xE] += r1;
    dmg[0xF] += r2;
    e.h(0xDDDE0, unit, 0x80, 0, 0, 0, game, tgt);
    e.edi9(0xDDDE0, dmg);
    e.h(0xDED30, 0, 0, 0, 0, 0, game, unit, tgt);
    return 1;
}
}
