// 1.13c D2Game+0x56950: eax = pGame; walks the quest list (pGame+0x10F4 holds the head pointer, nodes linked at +0xF4) for the node of quest 0x21; returns (node+0x18)+0x84 == 0 (0 when no list or no node).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_56950(uint32_t game)
{
    const uint32_t hdr = d2rd32(game + 0x10F4);
    if (!hdr) return 0;
    for (uint32_t n = d2rd32(hdr); n; n = d2rd32(n + 0xF4))
        if (d2rd32(n) == 0x21) return d2rd32(d2rd32(n + 0x18) + 0x84) == 0;
    return 0;
}
