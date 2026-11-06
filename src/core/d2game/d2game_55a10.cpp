// 1.13c D2Game+0x55A10 (item list section reader): EAX = remaining length, ECX = section, stack (a0, a1, version, pSize), ret 0x10.
// *pSize = 0 first. A version outside 0x47..0x60 returns 1; a first u16 other than 0x4D4A ('JM') returns 0xE. The u16 count at +2 gives the item count; each item
// is read by 0x550A0 (EAX = a1; stack a0, cursor, version, remaining, &consumed) with the cursor starting at section + 4 and the remaining length at EAX - 4, both
// advanced by the item's consumed length. A non-zero status is returned at once; otherwise *pSize = 4 + the sum of the consumed lengths and 0 is returned.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_55a10(uint32_t rem, uint32_t section, uint32_t a0, uint32_t a1, uint32_t version, uint32_t pSize, const D2Seam* e)
{
    d2wr32(pSize, 0);
    if (version < 0x47 || version > 0x60) return 1;
    if (d2rd16(section) != 0x4D4A) return 0xE;
    const uint32_t count = d2rd16(section + 2);
    uint32_t cursor = section + 4, left = rem - 4, total = 4;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t consumed = 0;
        const uint32_t w[5] = { a0, cursor, version, left, (uint32_t)(uintptr_t)&consumed };
        const uint32_t st = e->call(0x550A0, a1, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4]);
        e->blk(0x550A0, w, 5);
        if (st != 0) return st;
        total += consumed; cursor += consumed; left -= consumed;
    }
    d2wr32(pSize, total);
    return 0;
}
