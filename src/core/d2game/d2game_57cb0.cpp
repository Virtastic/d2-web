// 1.13c D2Game+0x57CB0: ecx = pGame, edx = param. Quest node 0x21's data d: d+0xB1 set returns 1; d+0x84 clear returns 0; else
// #A478(edx) then #1280() (ebx = d[0xC4]); a zero #1280 adds 3 to d[0xC4]/d[0xC8] and returns 0, otherwise #E1D90 (ecx 2, edx 0xBD,
// stack d[0xC4], d[0xC8], game, r1280, 1, 1, 0) spawns the portal: null returns 0, else d+0xB1 = 1, d[0xC0] = unit[0xC], return 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_57cb0(uint32_t game, uint32_t edx, const D2Seam* e)
{
    uint32_t l = d2rd32(game + 0x10F4);
    if (!l) return 0;
    uint32_t n = d2rd32(l);
    while (n && d2rd32(n) != 0x21) n = d2rd32(n + 0xF4);
    if (!n) return 0;
    const uint32_t d = d2rd32(n + 0x18);
    if (d2rd8(d + 0xB1)) return 1;
    if (!d2rd32(d + 0x84)) return 0;
    const uint32_t r = e->call(0xA478, 0, 0, 0, 0, 0, edx);
    const uint32_t x = e->call(0x1280, r, 0, 0, d2rd32(d + 0xC4));
    if (!x) { d2wr32(d + 0xC4, d2rd32(d + 0xC4) + 3); d2wr32(d + 0xC8, d2rd32(d + 0xC8) + 3); return 0; }
    const uint32_t u = e->call(0xE1D90, 0, 2, 0xBD, 0, 0, d2rd32(d + 0xC4), d2rd32(d + 0xC8), game, x, 1, 1, 0);
    if (!u) return 0;
    d2wr8(d + 0xB1, 1);
    d2wr32(d + 0xC0, d2rd32(u + 0xC));
    return 1;
}
