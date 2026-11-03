// 1.13c D2Game+0x92230: ecx = p1, edx = unit, stack a0, a1 (ret 8). 0x9FA30(eax = unit, &local[2]) must succeed and a1 be positive;
// then 0x91340(eax = a0, unit) must come back positive; the request 0xA0000(p1, a0, a1, r, local[1], local[0]) goes out and the answer is 1. Else 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_92230(uint32_t p1, uint32_t unit, uint32_t a0, int32_t a1, const D2Seam* e)
{
    uint32_t loc[2] = { 0, 0 };
    if (!e->call(0x9FA30, unit, 0, 0, 0, 0, (uint32_t)(uintptr_t)loc)) return 0;
    if (a1 <= 0) return 0;
    const int32_t r = (int32_t)e->call(0x91340, a0, 0, 0, 0, 0, unit);
    if (r <= 0) return 0;
    e->call(0xA0000, 0, 0, 0, 0, 0, p1, a0, (uint32_t)a1, (uint32_t)r, loc[1], loc[0]);
    return 1;
}
