// 1.13c D2Game+0x583C0: ecx = quest node, edx = ctx; tab = the dword table at D2Game+0x10BC20 (indexed by q[0xC]). #A4C0 = Ordinal_10920(ctx[0xC]); #A568 = Ordinal_10174; #33EB0 is a register callee (eax = kind, ecx = q, two stack words: ctx[0x14], the id C from ctx[8]+4 or -1).
#include "d2game_seam4e.h"
extern "C" void d2g_583c0(uint32_t q, uint32_t ctx, uint32_t tab, const D2Seam* e)
{
    const uint32_t q4 = d2rd32(q + 4);
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, d2rd32(ctx + 0xC));
    const uint32_t ent = d2rd32(tbl + 0x10 + d2rd8(q4 + 0x6D) * 4);
    const uint32_t A = e->call(0xA568, 0, 0, 0, 0, 0, ent, d2rd32(q + 0xE0), 0);
    const uint32_t B = e->call(0xA568, 0, 0, 0, 0, 0, ent, d2rd32(q + 0xE0), 0xD);
    const uint32_t u8 = d2rd32(ctx + 8);
    const uint32_t C = u8 ? d2rd32(u8 + 4) : 0xFFFFFFFFu;
    if (e->call(0xA568, 0, 0, 0, 0, 0, ent, d2rd32(q + 0xE0), 1)) { e->call(0x33EB0, 3, q, 0, 0, 0, d2rd32(ctx + 0x14), C); return; }
    const uint32_t cu = d2rd32(ctx + 0xC);
    const uint32_t id = cu ? d2rd32(cu + 0xC) : 0xFFFFFFFFu;
    const uint32_t cnt = d2rd16(q + 0x9C);
    for (uint32_t i = 0; i < cnt; ++i)
        if (d2rd32(q + 0x1C + i * 4) == id) { e->call(0x33EB0, 4, q, 0, 0, 0, d2rd32(ctx + 0x14), C); return; }
    if (A == 1) return;
    const uint32_t al = d2rd8(q + 0xC);
    if (al >= 4 && B == 0) return;
    if (!d2rd8(q + 9)) return;
    const uint32_t k = d2rd32(tab + al * 4);
    if (k == 2 && C == 0x203) {
        if (e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x24, 4)) { e->call(0x33EB0, 5, q, 0, 0, 0, d2rd32(ctx + 0x14), 0x203); return; }
    } else if (k == 0xFFFFFFFFu || k >= 0xC) return;
    e->call(0x33EB0, k, q, 0, 0, 0, d2rd32(ctx + 0x14), C);
}
