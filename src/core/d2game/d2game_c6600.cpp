// D2Game+0xC6600 -- ACT2Q5_OnSummonerActivated: EAX = game, plain ret. Written from the 1.13c disassembly; callees are the D2Seam.
// Quest list: game+0x10F4 points at a head pointer, nodes chain through +0xF4, kind at +0. The type-0xC node needs +9 == 1: its data block
// (node+0x18) byte +1 is set to 1 when clear; with +0xC clear 0x35EB0 (EAX = 1, ESI = node); with +0xB < 2: +0x14 = 0, +0xB = 2 and 0xDFCC0 twice
// (EAX 0, EBX callback 0xC4E30 then 0xC3010, EDI = node+4, stack 0).
#include "d2game_seam4e.h"
// 1.13c D2Game+0xC6600
extern "C" void d2game_c6600(uint32_t game, const D2Seam* e)
{
    const uint32_t list = d2rd32(game + 0x10F4);
    if (!list) return;
    uint32_t n = d2rd32(list);
    if (!n) return;
    while (d2rd32(n) != 0xC) { n = d2rd32(n + 0xF4); if (!n) return; }
    if (d2rd8(n + 9) != 1) return;
    const uint32_t d = d2rd32(n + 0x18);
    if (!d2rd8(d + 1)) d2wr8(d + 1, 1);
    if (!d2rd8(n + 0xC)) e->call(0x35EB0, 1, 0, 0, 0, n);
    if (d2rd8(n + 0xB) < 2) {
        d2wr8(n + 0x14, 0);
        d2wr8(n + 0xB, 2);
        e->call(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC4E30, d2rd32(n + 4), 0);
        e->call(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC3010, d2rd32(n + 4), 0);
    }
}
