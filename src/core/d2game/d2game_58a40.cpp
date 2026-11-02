// 1.13c D2Game+0x58A40: eax = unit, one stack arg = game (ret 4). #A4C0 = Ordinal_10920 (called when the unit's type dword is 0; result dead). The game's quest list (head at +0x10F4) is walked for quest 0x24 (not null-checked by retail); packet {0x50, word 0x24, word (quest data +0xA4)} goes out through #8A3E0 (ptr, 0xF).
#include "d2game_seam4e.h"
extern "C" void d2g_58a40(uint32_t unit, uint32_t game, const D2Seam* e)
{
    if (unit && d2rd32(unit) == 0) e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    uint32_t n = 0;
    const uint32_t hdr = d2rd32(game + 0x10F4);
    if (hdr) for (n = d2rd32(hdr); n; n = d2rd32(n + 0xF4)) if (d2rd32(n) == 0x24) break;
    const uint32_t w = d2rd16(d2rd32(n + 0x18) + 0xA4);
    uint32_t pk[4] = { 0x50u | (0x24u << 8) | ((w & 0xFF) << 24), w >> 8, 0, 0 };
    e->call(0x8A3E0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)pk, 0xF);
    e->blk(0x8A3E0, pk, 2);
}
