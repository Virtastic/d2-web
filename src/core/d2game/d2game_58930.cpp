// 1.13c D2Game+0x58930: ecx = quest node, edx = ctx. #A4C0 = Ordinal_10920 (called with ctx[0xC]; second call with the ctx[0xC] unit when its type dword is 0, result +0x9C is the data record); #A568 = Ordinal_10174 (getter); #A57A = Ordinal_10003 (setter). The record is not null-checked by retail (+0xA is read).
// Returns retail's EAX at exit (the function is void; the FN record compares it): the last getter result, the step factor, or the word written back.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_58930(uint32_t q, uint32_t ctx, const D2Seam* e)
{
    const uint32_t unit = d2rd32(ctx);
    const uint32_t cu = d2rd32(ctx + 0xC);
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, cu);
    const uint32_t ent = d2rd32(tbl + 0x10 + d2rd8(unit + 0x6D) * 4);
    const uint32_t st = d2rd32(q + 0xE0);
    if (!e->call(0xA568, 0, 0, 0, 0, 0, ent, st, 0) && !e->call(0xA568, 0, 0, 0, 0, 0, ent, st, 0xF)) {
        if (e->call(0xA568, 0, 0, 0, 0, 0, ent, st, 1) == 1) return 1;
        if (e->call(0xA568, 0, 0, 0, 0, 0, ent, st, 3) != 1) {
            const uint32_t r2 = e->call(0xA568, 0, 0, 0, 0, 0, ent, st, 2);
            if (r2 != 1) return r2;
            d2wr8(d2rd32(q + 0x18) + 0x90, 1);
        }
        d2wr8(q + 0xB, 1); d2wr8(q + 0xC, 3);
        return 1;
    }
    e->call(0xA57A, 0, 0, 0, 0, 0, ent, st, 0xA);
    uint32_t rec = 0;
    if (cu && d2rd32(cu) == 0) rec = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, cu) + 0x9C);
    const uint32_t idx = d2rd8(d2rd32(q + 4) + 0x6D);
    const uint32_t w = d2rd16(rec + 0xA);
    const uint32_t a = ((w & 0x20) | 0x80) >> 5;
    const int32_t v = (int32_t)(a * idx + 5);
    if (v < (int32_t)((w >> 8) & 0x1F)) return a;
    const uint32_t nw = ((uint32_t)v << 8) | (w & 0xE0FF);
    d2wr16(rec + 0xA, nw);
    return nw;
}
