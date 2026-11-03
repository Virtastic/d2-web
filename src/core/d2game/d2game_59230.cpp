// 1.13c D2Game+0x59230: eax = pGame, stack = unit (ret 4). Finds quest node 0x20 in the game's quest list; when its +9 flag is set the group
// index from #58260(eax = unit, ecx = node) (-1 = none) bumps the counter at (node+0x18)+0x114+group*4, and a counter >= 5 (signed) sets the byte at +0x108+group.
#include "d2game_seam4e.h"
extern "C" void d2g_59230(uint32_t game, uint32_t unit, const D2Seam* e)
{
    uint32_t l = d2rd32(game + 0x10F4);
    if (!l) return;
    uint32_t n = d2rd32(l);
    while (n && d2rd32(n) != 0x20) n = d2rd32(n + 0xF4);
    if (!n || !d2rd8(n + 9)) return;
    const int32_t g = (int32_t)e->call(0x58260, unit, n);
    if (g < 0) return;
    const uint32_t d = d2rd32(n + 0x18);
    const int32_t v = (int32_t)d2rd32(d + 0x114 + g * 4) + 1;
    d2wr32(d + 0x114 + g * 4, (uint32_t)v);
    if (v >= 5) d2wr8(d + 0x108 + g, 1);
}
