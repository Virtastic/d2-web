// 1.13c D2Game+0x3DEF0: the Meteor centre missile's server hit (ECX = game, EDX = missile, stack word = the hit record, ret 4; returns
// 1). `dt` is the value of sgptDataTables. With a missile row (class +0x04 in dt+0xB64, count dt+0xB6C, 0x1A4 per row), the missile's
// skill #B26A as a row of the skill table (dt+0xB98, count dt+0xBA0, 0x23C per row), its level #B0FC, flag bit 10 (+0xC8) and a target
// (0xE03A0: ECX = game, EAX = +0x94, EDX = +0x98): x, y = 0x1100 / 0x1130 (ECX = missile); the duration is row +0x4C, or when that is
// not positive #AC9A(target, skill row +0x64, skill, level) at least 1; a zeroed 28-dword damage block filled by 0x3A4E0(hit, &block),
// block +0 |= row +0xA8, word +4 |= row +0xAC; the area damage 0xA0C70 (EAX = game; target, x, y, duration, 0x8583, callback 0xA1210,
// &block, 0). When row word +0x24 is not negative the fire field follows: 0x3CEC0 (EAX = missile; game, row +0x24, 0x1A40(EAX = skill,
// EDX = level), max(1, row +0x50)).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_3def0(uint32_t game, uint32_t unit, uint32_t hit, uint32_t dt, const D2Seam* e)
{
    if (!unit) return 1;
    const int32_t cls = (int32_t)d2rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)d2rd32(dt + 0xB6C)) return 1;
    const uint32_t row = d2rd32(dt + 0xB64) + (uint32_t)cls * 0x1A4;
    if (!row) return 1;
    const uint32_t skill = e->call(0xB26A, 0, 0, 0, 0, 0, unit), lvl = e->call(0xB0FC, 0, 0, 0, 0, 0, unit);
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)d2rd32(dt + 0xBA0)) return 1;
    const uint32_t sr = d2rd32(dt + 0xB98) + skill * 0x23C;
    if (!sr || !((d2rd32(unit + 0xC8) >> 10) & 1)) return 1;
    const uint32_t tgt = e->call(0xE03A0, d2rd32(unit + 0x94), game, d2rd32(unit + 0x98));
    if (!tgt) return 1;
    const uint32_t x = e->call(0x1100, 0, unit), y = e->call(0x1130, 0, unit);
    int32_t dur = (int32_t)d2rd32(row + 0x4C);
    if (dur <= 0) {
        dur = (int32_t)e->call(0xAC9A, 0, 0, 0, 0, 0, tgt, d2rd32(sr + 0x64), skill, lvl);
        if (dur < 1) dur = 1;
    }
    uint32_t blk[28] = {};
    e->call(0x3A4E0, 0, 0, 0, 0, 0, hit, (uint32_t)(uintptr_t)blk);
    blk[0] |= d2rd32(row + 0xA8);
    blk[1] = (blk[1] & 0xFFFF0000u) | ((blk[1] | d2rd16(row + 0xAC)) & 0xFFFFu);
    e->call(0xA0C70, game, 0, 0, 0, 0, tgt, x, y, (uint32_t)dur, 0x8583, 0x6FCC1210, (uint32_t)(uintptr_t)blk, 0);
    e->blk(0xA0C70, blk, 28);
    const int16_t w = (int16_t)d2rd16(row + 0x24);
    if (w < 0) return 1;
    const uint32_t len = e->call(0x1A40, skill, 0, lvl);
    int32_t n = (int32_t)d2rd32(row + 0x50);
    if (n < 1) n = 1;
    e->call(0x3CEC0, unit, 0, 0, 0, 0, game, (uint32_t)(int32_t)w, len, (uint32_t)n);
    return 1;
}
