// 1.13c D2Game+0x3D8C0: the Molten Boulder missile's server hit (ECX = game, EDX = missile, stack word = the hit record, ret 4).
// `dt` is the value of sgptDataTables. Returns 1 unless a hit record is given that is not a unit hit of kind 1 whose unit passes
// 0x13B0 (EAX = hit +4, EDX = 11), which returns 2. With a missile row (class +0x04 in dt+0xB64, count dt+0xB6C, 0x1A4 per row),
// missile flag bit 10 (+0xC8) and a target (0xE03A0: ECX = game, EAX = +0x94, EDX = +0x98): x, y = 0x1100 / 0x1130 (ECX = missile),
// a zeroed 28-dword damage block filled by 0x3A4E0(hit, &block); the duration is row +0x4C, or when that is not positive the skill's
// (#B26A, #B0FC level, 0x1420 skill row, #AC9A(target, row +0x64, skill, level)) at least 1. Then block +0 |= row +0xA8, word +4 |=
// row +0xAC, the area damage 0xA0C70 (EAX = game; target, x, y, duration, 0x8583, callback 0xA1210, &block, 0), and the boulder's
// follow-up 0x3CEC0 (EAX = missile; game, (short) row +0x24, duration, max(1, row +0x50)).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_3d8c0(uint32_t game, uint32_t unit, uint32_t hit, uint32_t dt, const D2Seam* e)
{
    if (!unit) return 1;
    const int32_t cls = (int32_t)d2rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)d2rd32(dt + 0xB6C)) return 1;
    const uint32_t row = d2rd32(dt + 0xB64) + (uint32_t)cls * 0x1A4;
    if (!row || !((d2rd32(unit + 0xC8) >> 10) & 1)) return 1;
    const uint32_t tgt = e->call(0xE03A0, d2rd32(unit + 0x94), game, d2rd32(unit + 0x98));
    if (!tgt) return 1;
    if (hit && (d2rd32(hit) != 1 || !e->call(0x13B0, d2rd32(hit + 4), 0, 11))) return 2;
    const uint32_t x = e->call(0x1100, 0, unit), y = e->call(0x1130, 0, unit);
    uint32_t blk[28] = {};
    e->call(0x3A4E0, 0, 0, 0, 0, 0, hit, (uint32_t)(uintptr_t)blk);
    int32_t dur = (int32_t)d2rd32(row + 0x4C);
    if (dur <= 0) {
        const uint32_t skill = e->call(0xB26A, 0, 0, 0, 0, 0, unit), lvl = e->call(0xB0FC, 0, 0, 0, 0, 0, unit);
        const uint32_t sr = e->call(0x1420, skill);
        if (!sr) return 1;
        dur = (int32_t)e->call(0xAC9A, 0, 0, 0, 0, 0, tgt, d2rd32(sr + 0x64), skill, lvl);
        if (dur < 1) dur = 1;
    }
    blk[0] |= d2rd32(row + 0xA8);
    blk[1] = (blk[1] & 0xFFFF0000u) | ((blk[1] | d2rd16(row + 0xAC)) & 0xFFFFu);
    e->call(0xA0C70, game, 0, 0, 0, 0, tgt, x, y, (uint32_t)dur, 0x8583, 0x6FCC1210, (uint32_t)(uintptr_t)blk, 0);
    e->blk(0xA0C70, blk, 28);
    int32_t n = (int32_t)d2rd32(row + 0x50);
    if (n < 1) n = 1;
    e->call(0x3CEC0, unit, 0, 0, 0, 0, game, (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x24), (uint32_t)dur, (uint32_t)n);
    return 1;
}
