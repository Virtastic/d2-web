// 1.13c D2Game+0xACE60: event 0x07 roll, ecx = game, edx = event, stack attacker, unit, damage, packed, arg5 (ret 0x14). bm = gdwBitMasks (IAT 0xF8BE0).
// attacker, unit, damage non-null and #A802(attacker, packed>>16, packed) > 0; threshold 0x40 (monster row byte +5 & bm[3]: 0x20, & bm[2]: 0x80); seed step at attacker+0x20;
// (new low & 0x7F) < threshold sets damage+4 |= 8 and returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ace60(uint32_t bm, uint32_t attacker, uint32_t unit, uint32_t dmg, uint32_t packed, const D2Seam* e)
{
    if (!attacker || !unit || !dmg) return 0;
    if ((int32_t)e->call(0xA802, 0, 0, 0, 0, 0, attacker, packed >> 16, packed) <= 0) return 0;
    uint32_t th = 0x40;
    if (d2rd32(unit) == 1) {
        const uint32_t row = e->call(0x1320, d2rd32(unit + 4));
        if (row) {
            const uint32_t b = d2rd8(row + 5);
            if (d2rd32(bm + 0xC) & b) th = 0x20;
            else if (d2rd32(bm + 8) & b) th = 0x80;
        }
    }
    const uint64_t s = (uint64_t)d2rd32(attacker + 0x20) * 0x6AC690C5ull + d2rd32(attacker + 0x24);
    d2wr32(attacker + 0x20, (uint32_t)s);
    d2wr32(attacker + 0x24, (uint32_t)(s >> 32));
    if (((uint32_t)s & 0x7F) >= th) return 0;
    d2wr8(dmg + 4, d2rd8(dmg + 4) | 8);
    return 1;
}
