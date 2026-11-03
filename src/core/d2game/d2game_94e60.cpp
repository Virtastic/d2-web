// 1.13c D2Game+0x94E60: ecx = c, edx = unit, stack a0 (idx), a1 (ret 8). The unit's 64-bit seed (unit+0x20 lo, +0x24 hi) advances first: lo*0x6ac690c5 + hi.
// idx picks a skills row (dt+0xB98, stride 0x23C, count dt+0xBA0); 0x91340(eax = idx, unit) must be positive; 0x9FA30(eax = unit, ecx = c, ebx = &X; &a0 slot) must succeed
// (it rewrites X and the a0 slot). The level is 0x91240(ebx = unit, edi = idx; a1) or, when that is 0, 0xAC9A(unit, row+0x64, idx, a1); 0x93D60 (edx = seed lo;
// c, unit, r, X, a0, idx, a1, level) closes it and the answer is 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_94e60(uint32_t c, uint32_t unit, uint32_t a0, uint32_t a1, uint32_t dt, const D2Seam* e)
{
    const uint64_t sd = (uint64_t)d2rd32(unit + 0x20) * 0x6ac690c5u + d2rd32(unit + 0x24);
    const uint32_t lo = (uint32_t)sd;
    d2wr32(unit + 0x24, (uint32_t)(sd >> 32)); d2wr32(unit + 0x20, lo);
    const uint32_t idx = a0;
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    const uint32_t r = e->call(0x91340, idx, 0, 0, 0, 0, unit);
    if ((int32_t)r <= 0) return 0;
    uint32_t X = 0;
    if (!e->call(0x9FA30, unit, c, 0, (uint32_t)(uintptr_t)&X, 0, (uint32_t)(uintptr_t)&a0)) return 0;
    uint32_t v = e->call(0x91240, 0, 0, 0, unit, 0, a1);
    if (!v) v = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x64), idx, a1);
    e->call(0x93D60, 0, 0, lo, 0, 0, c, unit, r, X, a0, idx, a1, v);
    return 1;
}
