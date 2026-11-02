// 1.13c D2Game+0x58C90: ecx = object g (+0x6D byte index), edx = ctx unit, one unused stack word (ret 4). #A4C0 = Ordinal_10920; #A568 = Ordinal_10174 (getter); #A478 = Ordinal_10331; #A556; #A56E (low byte compared with 4); #A57A = Ordinal_10003 (setter); #4A2D0 is a register callee (edx = index byte, esi = record or 0, edi = 5).
#include "d2game_seam4e.h"
extern "C" void d2g_58c90(uint32_t g, uint32_t ctx, const D2Seam* e)
{
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, ctx);
    const uint32_t ent = d2rd32(tbl + 0x10 + d2rd8(g + 0x6D) * 4);
    if (e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x28, 0) == 1) return;
    if (e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x28, 1) == 1) return;
    uint32_t x = e->call(0xA478, 0, 0, 0, 0, 0, ctx);
    if (!x) return;
    x = e->call(0xA556, 0, 0, 0, 0, 0, x);
    if (!x) return;
    if ((e->call(0xA56E, 0, 0, 0, 0, 0, x) & 0xFF) != 4) return;
    e->call(0xA57A, 0, 0, 0, 0, 0, ent, 0x28, 0);
    e->call(0xA57A, 0, 0, 0, 0, 0, ent, 0x28, 0xD);
    uint32_t rec = 0;
    if (ctx && d2rd32(ctx) == 0) rec = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, ctx) + 0x9C);
    e->call(0x4A2D0, 0, 0, d2rd8(g + 0x6D), 0, rec);
}
