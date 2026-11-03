// 1.13c D2Game+0x954F0: ecx = this, edx = p (plain ret, always returns 1). this+0 = x0, this+4 = unit, this+0xC = B; 0x95120(eax = B, ecx = p; x0, unit) must come back
// 0, then the 0x70-byte request block at [B] is copied to a local and goes to 0xDE0C0(eax = p; x0, unit, 1, &block) and 0xDCF20(eax = p; x0, unit, &block).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_954f0(uint32_t t, uint32_t p, const D2Seam* e)
{
    const uint32_t x0 = d2rd32(t), un = d2rd32(t + 4), B = d2rd32(t + 0xC);
    if (e->call(0x95120, B, p, 0, 0, 0, x0, un)) return 1;
    uint32_t q[28];
    for (int i = 0; i < 28; i++) q[i] = d2rd32(d2rd32(B) + 4 * i);
    e->call(0xDE0C0, p, 0, 0, 0, 0, x0, un, 1, (uint32_t)(uintptr_t)q);
    e->blk(0xDE0C0, q, 28);
    e->call(0xDCF20, p, 0, 0, 0, 0, x0, un, (uint32_t)(uintptr_t)q);
    e->blk(0xDCF20, q, 28);
    return 1;
}
