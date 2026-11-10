// 1.13c D2Game+0x58040: ecx = quest node, edx = ctx. #33830 (register callee: eax = unit id or -1 when ctx[0xC] is null, ecx = target) on q+0x1C then on q[0x18]; then #E660(eax = ctx[0xC], 0x20656369): nonzero decrements (q[0x18]+0xD8).
#include "d2game_seam4e.h"
static uint32_t uid(uint32_t ctx) { const uint32_t u = d2rd32(ctx + 0xC); return u ? d2rd32(u + 0xC) : 0xFFFFFFFFu; }
extern "C" uint32_t d2g_58040(uint32_t q, uint32_t ctx, const D2Seam* e)
{
    e->call(0x33830, uid(ctx), q + 0x1C);
    const uint32_t d = d2rd32(q + 0x18);
    e->call(0x33830, uid(ctx), d);
    // retail returns the 0xE660 result in EAX (the last call before the epilogue)
    const uint32_t r = e->call(0xE660, d2rd32(ctx + 0xC), 0, 0, 0, 0, 0x20656369);
    if (r) d2wr32(d + 0xD8, d2rd32(d + 0xD8) - 1);
    return r;
}
