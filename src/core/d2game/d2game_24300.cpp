// 1.13c D2Game+0x24300: ECX = game, EDX = unit; two ignored stack words. Monster area-damage burst: the monster row (dt+0xA78, 0x1A8 each,
// index unit[+4], none when out of range or the unit is null) gives lo = int16 row[0x10E], hi = int16 row[0x120]; with a positive span the unit seed
// (+0x20) steps (lo * 0x6AC690C5 + hi) and the roll is the low word mod span. The damage word is (lo + roll) << 8; the scale is #A42A(unit, 0xC, 0)
// * int16 row[0x62 + 2 * byte game[0x6D]]. A 28-dword block with [4] = the damage word and the unit position (types 2, 4..5 static) go to 0xDE380
// (ten stack words: game, unit, x, y, scale, block, 0 x4; EBX = damage word, ESI = unit).
#include "d2game_f1pos.h"
extern "C" void d2g_24300(uint32_t game, uint32_t unit, uint32_t dt, const D2Seam* e)
{
    uint32_t row = 0;
    if (unit) {
        const int32_t cls = (int32_t)d2rd32(unit + 4);
        if (cls >= 0 && cls < (int32_t)d2rd32(dt + 0xA80)) row = d2rd32(dt + 0xA78) + (uint32_t)cls * 0x1A8;
    }
    const int32_t lo = (int16_t)d2rd16(row + 0x10E), span = (int16_t)d2rd16(row + 0x120) - lo;
    uint32_t roll = 0;
    if (span > 0) {
        const uint64_t n = (uint64_t)d2rd32(unit + 0x20) * 0x6AC690C5u + d2rd32(unit + 0x24);
        d2wr32(unit + 0x24, (uint32_t)(n >> 32)); d2wr32(unit + 0x20, (uint32_t)n);
        roll = (uint32_t)n % (uint32_t)span;
    }
    const uint32_t dmg = (uint32_t)(lo + (int32_t)roll) << 8;
    const int32_t level = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xc, 0);
    const uint32_t scale = (uint32_t)(level * (int32_t)(int16_t)d2rd16(row + 0x62 + 2 * d2rd8(game + 0x6D)));
    uint32_t q[28] = {};
    q[4] = dmg;
    const uint32_t x = f1PosX(unit, e), y = f1PosY(unit, e);
    e->call(0xDE380, 0, 0, 0, dmg, unit, game, unit, x, y, scale, (uint32_t)(uintptr_t)q);
    e->blk(0xDE380, q, 28);
}
