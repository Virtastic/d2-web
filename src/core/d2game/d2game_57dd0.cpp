// 1.13c D2Game+0x57DD0: ecx = quest node, edx = ctx. #E660(eax = ctx[0xC], 0x20656369) internal; #A4C0 = Ordinal_10920; #A568 = Ordinal_10174 (getter); #A57A = Ordinal_10003 (setter).
// Returns retail's EAX at exit (the function is void; the FN record compares it): the #E660 result (or q[0x18] when set) with q[9] in its low byte, 0, node 0x22, or the last getter/setter result.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_57dd0(uint32_t q, uint32_t ctx, const D2Seam* e)
{
    uint32_t ax = e->call(0xE660, d2rd32(ctx + 0xC), 0, 0, 0, 0, 0x20656369);
    if (ax) {
        ax = d2rd32(q + 0x18);
        d2wr32(ax + 0xD8, d2rd32(ax + 0xD8) + 1);
    }
    ax = (ax & 0xFFFFFF00u) | d2rd8(q + 9);
    if (d2rd8(q + 9) && d2rd8(q + 0xC) < 5) return ax;
    const uint32_t hdr = d2rd32(d2rd32(q + 4) + 0x10F4);
    if (!hdr) return 0;
    uint32_t n = d2rd32(hdr);
    if (!n) return 0;
    while (d2rd32(n) != 0x22) { n = d2rd32(n + 0xF4); if (!n) return 0; }
    if (!d2rd8(n + 9)) return n;
    const uint32_t unit = d2rd32(ctx);
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, d2rd32(ctx + 0xC));
    const uint32_t ent = d2rd32(tbl + 0x10 + d2rd8(unit + 0x6D) * 4);
    uint32_t r;
    if ((r = e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x25, 1))) return r;
    if ((r = e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x25, 6))) return r;
    if ((r = e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x25, 0))) return r;
    return e->call(0xA57A, 0, 0, 0, 0, 0, ent, 0x26, 0xE);
}
