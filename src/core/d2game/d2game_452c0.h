// 1.13c D2Game+0x452C0 (missile-spawning skill start from a skill id), transcribed from the 1.13c disassembly. Convention:
// ECX = game, EDX = unit, stack = (skill id, skill level), ret 8. Every callee is one E::h(rva, eax, ecx, edx, ebx, esi, stack
// words...) call as retail passes it. 0x9FA30 writes two results: through EBX (a caller local) and through its stack word 0, which
// retail points at the caller's OWN first argument slot, so the id the request block and 0x7A190 see afterwards is that result.
// The 23-dword missile request (0x5C bytes, fully zeroed first) goes to 0x6F930 in EDX and is logged by blk().
#pragma once
#include <cstdint>

namespace r452c0 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, int32_t id, uint32_t level)
{
    if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    if (!((uint32_t)id * 0x23C + e.rd32(dt + 0xB98))) return 0;
    const int32_t sk = (int32_t)e.h(0x91340, (uint32_t)id, 0, 0, 0, 0, unit);
    if (sk <= 0) return 0;
    volatile uint32_t out0 = 0, slot = (uint32_t)id;
    if (!e.h(0x9FA30, unit, game, 0, e.stk((const void*)&out0), 0, e.stk((const void*)&slot))) return 0;
    if (!e.h(0xA0630, (uint32_t)sk, 0, 0, 0, 0, game, unit)) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    const uint32_t rnd = e.h(0x1180, 0x100, unit + 0x20) & 0xFF;
    uint32_t b[23] = {};
    b[0] = 1;
    b[1] = unit;
    b[4] = (uint32_t)sk;
    b[5] = out0;
    b[6] = slot;
    b[11] = (uint32_t)id;
    b[12] = level;
    const uint32_t m = e.h(0x6F930, 0, game, e.stk(b));
    e.blk(0x6F930, b, 23);
    if (!m) return 0;
    e.h(0xA50E, 0, 0, 0, 0, 0, m, rnd);
    e.h(0x7A190, 0, 0, 0, 0, 0, (uint32_t)id, level, out0, slot, 0);
    return 1;
}
}
