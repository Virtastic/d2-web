// 1.13c D2Game+0xAC9E0: eax = holder, edi = key. Walks the list at holder+0x60 (#A3DC first, #A4CC next); #A46C(node) == key returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ac9e0(uint32_t holder, uint32_t key, const D2Seam* e)
{
    const uint32_t head = d2rd32(holder + 0x60);
    if (!head) return 0;
    for (uint32_t n = e->call(0xA3DC, 0, 0, 0, 0, 0, head); n; n = e->call(0xA4CC, 0, 0, 0, 0, 0, n))
        if (e->call(0xA46C, 0, 0, 0, 0, 0, n) == key) return 1;
    return 0;
}
