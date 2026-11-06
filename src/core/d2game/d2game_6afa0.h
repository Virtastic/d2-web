// 1.13c D2Game+0x6AFA0 (sub_6AFA0): EAX = arg of 0xAE74, EBX = pUnit, [ebp+8] = p, [ebp+0xC] = s (ret 8).
// Written from the disassembly as a rule over an environment E (hookenv.h). 0xAE74(eax) == -1 returns; 0xA544(unit,&t); 0xA478(unit,&x,1) fills two
// dwords x; 0xE930(a478 result, ESI = &local) == 0 returns; then a 33-dword zeroed request: [0] unit, [2] p, [3] 0xE5D0(unit, 0), [5] AE74 result,
// [6] 3, [7] x0, [8] x1, [9] E930 result, word at +0x28 = 1, word at +0x2A = word [p+0x78], [12] s; finally 0x11490(p, &req, 0).
#pragma once
#include <cstdint>

namespace d2g6afa0 {
template <class E> void Run(E& e, uint32_t eax, uint32_t unit, uint32_t p, uint32_t s)
{
    const uint32_t id = e.h(0xAE74, 0, 0, 0, 0, 0, eax);
    if (id == 0xFFFFFFFFu) return;
    uint32_t t[2] = {}, x[2] = {};
    e.h(0xA544, 0, 0, 0, 0, 0, unit, e.stk(t));
    const uint32_t g = e.h(0xA478, 0, 0, 0, 0, 0, unit, e.stk(x), 1);
    uint32_t loc[2] = {};
    const uint32_t r = e.h(0xE930, 0, 0, 0, 0, e.stk(loc), g);
    if (!r) return;
    uint32_t req[0x21] = {};
    req[0] = unit; req[2] = p; req[5] = id; req[6] = 3; req[7] = x[0]; req[8] = x[1]; req[9] = r; req[12] = s;
    req[10] = 1u | (uint32_t)(e.rd32(p + 0x78) & 0xFFFFu) << 16;
    req[3] = e.h(0xE5D0, unit, 0, 0, 0, 0, 0);
    e.h(0x11490, 0, 0, 0, 0, 0, p, e.stk(req), 0);
    e.blk(0x11490, req, 0x21);
}
}  // namespace d2g6afa0
