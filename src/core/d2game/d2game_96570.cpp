// 1.13c D2Game+0x96570: eax = unit, ebx = guid (plain ret). q = [0xA4C0(unit) +0x44] (0: return 0); rows 1 .. dt+0xBF0 - 1 (12 bytes each, [q] + 12*row) head lists whose
// nodes carry the guid at +4 (next +0x14); the first match returns node + 8, none returns 0. A null unit record is fatal (untestable).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_96570(uint32_t unit, uint32_t guid, uint32_t dt, const D2Seam* e)
{
    const uint32_t r = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t q = d2rd32(r + 0x44);
    if (!q) return 0;
    for (int32_t i = 1; i < (int32_t)d2rd32(dt + 0xBF0); i++) {
        const uint32_t row = d2rd32(q) + i * 12;
        if (!row) continue;
        for (uint32_t n = d2rd32(row); n; n = d2rd32(n + 0x14))
            if (d2rd32(n + 4) == guid) return n + 8;
    }
    return 0;
}
