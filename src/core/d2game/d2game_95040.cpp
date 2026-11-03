// 1.13c D2Game+0x95040: ecx = c, edx = unit, stack a0 (idx), a1 (ret 8). 0xE07A0(eax = unit, ecx = c); target = 0xA598(unit +0x2C); a null or self target, a1 <= 0, an idx outside
// the skills table (dt+0xB98, stride 0x23C, count dt+0xBA0) or 0x91340(eax = idx, unit) <= 0 all return 0. Otherwise sum = 0xACFA(r, a1) + 0xAC9A(unit, row +0x138, idx, a1)
// and 0xA2800 (eax = target, ebx = sum; c, unit, r, idx, a1) runs; returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_95040(uint32_t c, uint32_t unit, uint32_t idx, int32_t a1, uint32_t dt, const D2Seam* e)
{
    e->call(0xE07A0, unit, c);
    const uint32_t t = e->call(0xA598, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    if (t == unit || !t) return 0;
    if (a1 <= 0) return 0;
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    const int32_t r = (int32_t)e->call(0x91340, idx, 0, 0, 0, 0, unit);
    if (r <= 0) return 0;
    uint32_t sum = e->call(0xACFA, 0, 0, 0, 0, 0, (uint32_t)r, (uint32_t)a1);
    sum += e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(row + 0x138), idx, (uint32_t)a1);
    e->call(0xA2800, t, 0, 0, sum, 0, c, unit, (uint32_t)r, idx, (uint32_t)a1);
    return 1;
}
