// 1.13c D2Game+0x57E80: ecx = quest node, edx = ctx. Callees (retail order): #E660(eax = ctx[0xC], 0x20656369) internal; #A4C0 = Ordinal_10920(ctx[0xC]) (table, entry at +0x10 + ctx[0]->+0x6D*4); #A568 = Ordinal_10174(entry, q[0xE0], state) x2..4.
// Returns retail's EAX at exit (the function is void; the FN record compares it): the deciding #A568 result, 0 when q[9] is clear, else d[0xD8].
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_57e80(uint32_t q, uint32_t ctx, const D2Seam* e)
{
    const uint32_t d = d2rd32(q + 0x18);
    if (e->call(0xE660, d2rd32(ctx + 0xC), 0, 0, 0, 0, 0x20656369)) d2wr32(d + 0xD8, 1);
    const uint32_t unit = d2rd32(ctx);
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, d2rd32(ctx + 0xC));
    const uint32_t ent = d2rd32(tbl + 0x10 + d2rd8(unit + 0x6D) * 4);
    uint32_t r = e->call(0xA568, 0, 0, 0, 0, 0, ent, d2rd32(q + 0xE0), 0);
    if (!r) r = e->call(0xA568, 0, 0, 0, 0, 0, ent, d2rd32(q + 0xE0), 0xF);
    if (r) { d2wr32(d + 0x84, 2); d2wr32(d + 0x88, 1); return r; }
    if (!d2rd8(q + 9)) return 0;
    if (e->call(0xA568, 0, 0, 0, 0, 0, ent, d2rd32(q + 0xE0), 3)) { d2wr8(q + 0xC, 3); d2wr8(q + 0xB, 1); }
    else if (e->call(0xA568, 0, 0, 0, 0, 0, ent, d2rd32(q + 0xE0), 2)) { d2wr8(q + 0xC, 2); d2wr8(q + 0xB, 1); }
    const uint32_t d8 = d2rd32(d + 0xD8);
    if (d8) { d2wr8(q + 0xC, 3); d2wr8(q + 0xB, 4); }
    return d8;
}
