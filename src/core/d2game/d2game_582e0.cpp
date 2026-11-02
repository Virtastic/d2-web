// 1.13c D2Game+0x582E0: ecx = pGame-like object (+0x6D byte index, +0x10F4 quest list head), edx = ctx arg, one unused stack word (ret 4). #A4C0 = Ordinal_10920(edx); #A568 = Ordinal_10174 (getter); #A478 = Ordinal_10331; #A556 and #A56E (low byte compared with 4); #A57A = Ordinal_10003 (setter). The quest 0x20 node is not null-checked by retail.
#include "d2game_seam4e.h"
extern "C" void d2g_582e0(uint32_t g, uint32_t arg, const D2Seam* e)
{
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, arg);
    const uint32_t ent = d2rd32(tbl + 0x10 + d2rd8(g + 0x6D) * 4);
    if (e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x24, 0) == 1) return;
    if (e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x24, 1) == 1) return;
    uint32_t x = e->call(0xA478, 0, 0, 0, 0, 0, arg);
    if (!x) return;
    x = e->call(0xA556, 0, 0, 0, 0, 0, x);
    if (!x) return;
    if ((e->call(0xA56E, 0, 0, 0, 0, 0, x) & 0xFF) != 4) return;
    e->call(0xA57A, 0, 0, 0, 0, 0, ent, 0x24, 0xD);
    e->call(0xA57A, 0, 0, 0, 0, 0, ent, 0x24, 1);
    uint32_t n = 0;
    const uint32_t hdr = d2rd32(g + 0x10F4);
    if (hdr) for (n = d2rd32(hdr); n; n = d2rd32(n + 0xF4)) if (d2rd32(n) == 0x20) break;
    const uint32_t v = d2rd32(d2rd32(n + 0x18) + 0xAC);
    e->call(0xA57A, 0, 0, 0, 0, 0, ent, 0x24, v == 0xF ? 5 : v == 0xE ? 6 : 7);
}
