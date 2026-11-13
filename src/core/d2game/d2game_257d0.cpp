// 1.13c D2Game+0x257D0: ECX = game, EDX = unit; two ignored stack words. Monster missile and area-damage burst. Row = dt+0xA78 + 0x1A8 * unit[+4] (none out of range);
// gb = byte game[0x6D]; sel = int16 row[0x56 + 2 * gb]; f = 1 when game[0x6A] or game[0x74] is set. #AD7E(class, f, gb, #A42A(unit, 0xC, 0), 8, &blk) fills a 14-dword
// block whose dwords 5 and 6 are lo and hi; with hi > lo the unit seed (+0x20) rolls 0..hi-lo-1 (steps lo * 0x6AC690C5 + hi, EBX keeps the old seed hi). dmg = (roll + lo) << 8
// goes into a 28-dword block: [1] = 8, [2] = dmg; sel 1 adds [4] = dmg and missile 0x1AB, sel >= 2 adds [9] = dmg, [12] = sel and 0x1AC, otherwise 0x1AA.
// #A28F0 (ECX = game, EDX = missile id, EBX = old seed hi or gb, ESI = unit; unit, 0, 1, 0, 0, x, y, 1) spawns it at the unit position (y read first); a missile
// gets #DE380 (EBX = x, ESI = row; game, missile, x, y, int16 row[0x68 + 2 * gb], &block, 0, 0, 0, 0x581) at the position read again (x, then y).
#include "d2game_f1pos.h"
extern "C" void d2g_257d0(uint32_t game, uint32_t unit, uint32_t dt, const D2Seam* e)
{
    uint32_t row = 0;
    if (unit) {
        const int32_t cls = (int32_t)d2rd32(unit + 4);
        if (cls >= 0 && cls < (int32_t)d2rd32(dt + 0xA80)) row = d2rd32(dt + 0xA78) + (uint32_t)cls * 0x1A8;
    }
    const uint32_t gb = d2rd8(game + 0x6D);
    const int32_t sel = (int16_t)d2rd16(row + 0x56 + 2 * gb);
    const uint32_t f = (d2rd8(game + 0x6A) || d2rd32(game + 0x74)) ? 1 : 0;
    uint32_t blk1[14] = {};
    const uint32_t cls = unit ? d2rd32(unit + 4) : 0xFFFFFFFFu;
    const uint32_t level = e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xc, 0);
    e->call(0xAD7E, 0, 0, 0, 0, 0, cls, f, gb, level, 8, (uint32_t)(uintptr_t)blk1);
    { const uint32_t w[2] = { 8, (uint32_t)(uintptr_t)blk1 }; e->x(0xAD7E, w, 2); }
    const uint32_t lo = blk1[5];
    const int32_t span = (int32_t)(blk1[6] - lo);
    uint32_t roll = 0, ebx = gb;
    if (span > 0) {
        ebx = d2rd32(unit + 0x24);
        const uint64_t n = (uint64_t)d2rd32(unit + 0x20) * 0x6AC690C5u + ebx;
        d2wr32(unit + 0x24, (uint32_t)(n >> 32)); d2wr32(unit + 0x20, (uint32_t)n);
        roll = (uint32_t)n % (uint32_t)span;
    }
    const uint32_t dmg = (roll + lo) << 8;
    uint32_t blk2[28] = {};
    blk2[1] |= 8; blk2[2] = dmg;
    uint32_t id;
    if (sel == 1) { blk2[4] = dmg; id = 0x1AB; }
    else if (sel >= 2) { blk2[12] = (uint32_t)sel; blk2[9] = dmg; id = 0x1AC; }
    else id = 0x1AA;
    const uint32_t y0 = f1PosY(unit, e), x0 = f1PosX(unit, e);
    const uint32_t missile = e->call(0xA28F0, 0, game, id, ebx, unit, unit, 0, 1, 0, 0, x0, y0, 1);
    const uint32_t x1 = f1PosX(unit, e), y1 = f1PosY(unit, e);
    if (!missile) return;
    const int32_t rad = (int16_t)d2rd16(row + 0x68 + 2 * gb);
    const uint32_t w[10] = { game, missile, x1, y1, (uint32_t)rad, (uint32_t)(uintptr_t)blk2, 0, 0, 0, 0x581 };
    e->call(0xDE380, 0, 0, 0, x1, row, w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
    e->x(0xDE380, w + 4, 6);
    e->blk(0xDE380, blk2, 28);
}
