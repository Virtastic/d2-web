// 1.13c D2Game+0x96280: ecx = c, edx = unit, stack idx, a1 (ret 8). idx picks a skills row (dt+0xB98, stride 0x23C, count dt+0xBA0), else 0. A unit gets bit 0x40 on unit +0xC4.
// 0x9FA30(eax = unit, ecx = c, ebx = &X; &idx slot) must succeed (it rewrites X and the idx slot Y); the answer is whether 0x95D50(eax = unit; c, X, Y, row, idx, a1) is nonzero.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_96280(uint32_t c, uint32_t unit, uint32_t idx, uint32_t a1, uint32_t dt, const D2Seam* e)
{
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    if (unit) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    uint32_t X = 0, Y = idx;
    if (!e->call(0x9FA30, unit, c, 0, (uint32_t)(uintptr_t)&X, 0, (uint32_t)(uintptr_t)&Y)) return 0;
    return e->call(0x95D50, unit, 0, 0, 0, 0, c, X, Y, row, idx, a1) != 0;
}
