// 1.13c D2Game+0x23320: kick a unit into its next mode. Fastcall ECX = game, EDX = unit, two stack words that are never read (ret 8).
// A null unit or one with bit 0 of the byte at +0xC6 does nothing. Type 0 acts when its +0x10 word is neither 0 nor 0x11; type 1 acts when
// that word is neither 0 nor 0xC; any other type does nothing. Acting runs 0xAF920(game) and then 0x8EA80(game, unit; 7, game[+0xA8] + 0x4B, 0, 0, 0).
#include "d2game_seam4e.h"
extern "C" void d2g_23320(uint32_t game, uint32_t unit, const D2Seam* e)
{
    if (!unit || (d2rd8(unit + 0xC6) & 1)) return;
    const uint32_t t = d2rd32(unit);
    const uint32_t s = d2rd32(unit + 0x10);
    if (t == 0) { if (!s || s == 0x11) return; }
    else if (t == 1) { if (!s || s == 0xC) return; }
    else return;
    e->call(0xAF920, game);
    e->call(0x8EA80, 0, game, unit, 0, 0, 7, d2rd32(game + 0xA8) + 0x4B, 0, 0, 0);
}
