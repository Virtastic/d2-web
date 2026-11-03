// 1.13c D2Game+0x96A40: eax = unit, esi = idx, ebx = any, edi = game (plain ret). Finds the node of row idx (pet table [0xA4C0(unit) +0x44], 12-byte rows, count dt+0xBF0) -- the first
// one when ebx is set, else the first with flag bit 0 clear -- and returns the unit in the game's bucket (edi +0x1320 + (id & 0x7F)*4, chain +0xE4, key +0xC) that carries its id +4, else 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_96a40(uint32_t unit, uint32_t idx, uint32_t any, uint32_t game, uint32_t dt, const D2Seam* e)
{
    const uint32_t r = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (!r || (int32_t)idx < 0 || (int32_t)idx >= (int32_t)d2rd32(dt + 0xBF0)) return 0;
    const uint32_t q = d2rd32(r + 0x44);
    if (!q) return 0;
    const uint32_t row = d2rd32(q) + idx * 12;
    if (!row) return 0;
    uint32_t n = d2rd32(row);
    if (!n) return 0;
    while (!any && (d2rd8(n) & 1)) { n = d2rd32(n + 0x14); if (!n) return 0; }
    const uint32_t id = d2rd32(n + 4);
    uint32_t u = d2rd32(game + 0x1320 + (id & 0x7F) * 4);
    while (u && d2rd32(u + 0xC) != id) u = d2rd32(u + 0xE4);
    return u;
}
