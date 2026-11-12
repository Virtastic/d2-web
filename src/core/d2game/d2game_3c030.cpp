// 1.13c D2Game+0x3C030: the Blade Fury missile's server hit (ECX = game, EDX = missile, one unused stack word, ret 4; returns 1).
// `dt` is the value of sgptDataTables. With a missile whose class (+0x04) is a row of the missile table (dt+0xB64, count dt+0xB6C,
// 0x1A4 per row) whose word +0x24 is not negative, and with missile flag bit 10 (+0xC8) set, the missile's target (0xE03A0: ECX =
// game, EAX = type +0x94, EDX = id +0x98) launches a ring of missiles: a zeroed 23-dword request (+0 = 2, +4 = target, +8 = missile,
// +0x2C = #B26A(missile), +0x30 = #B0FC(missile)) goes to 0xF6930's creator 0x6F930 (ECX = game, EDX = request) once per direction
// of the 8-entry offset tables at 0xFC450 (x, request +0x20) and 0xFC470 (y, +0x1C), stepping max(1, row +0x4C) directions at a time;
// each missile made gets #A50E(missile, y) and #AF5E(missile, x).
#include "d2game_seam4e.h"
static const int32_t kRingX[8] = { 0, 16, 16, 16, 0, -16, -16, -16 };   // D2Game 0xFC450
static const int32_t kRingY[8] = { 16, 16, 0, -16, -16, -16, 0, 16 };   // D2Game 0xFC470
extern "C" uint32_t d2g_3c030(uint32_t game, uint32_t unit, uint32_t dt, const D2Seam* e)
{
    if (!unit) return 1;
    const int32_t cls = (int32_t)d2rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)d2rd32(dt + 0xB6C)) return 1;
    const uint32_t row = d2rd32(dt + 0xB64) + (uint32_t)cls * 0x1A4;
    if (!row || (int16_t)d2rd16(row + 0x24) < 0) return 1;
    if (!((d2rd32(unit + 0xC8) >> 10) & 1)) return 1;
    const uint32_t tgt = e->call(0xE03A0, d2rd32(unit + 0x94), game, d2rd32(unit + 0x98));
    if (!tgt) return 1;
    uint32_t q[23] = {};
    q[0] = 2; q[1] = tgt; q[2] = unit;
    q[11] = e->call(0xB26A, 0, 0, 0, 0, 0, unit);
    q[12] = e->call(0xB0FC, 0, 0, 0, 0, 0, unit);
    int32_t step = (int32_t)d2rd32(row + 0x4C);
    if (step < 1) step = 1;
    for (uint32_t off = 0; (int32_t)off < 0x20; off += (uint32_t)step * 4) {   // retail steps a byte offset: step * 4 wraps at 32 bits
        const uint32_t i = off / 4;
        q[8] = (uint32_t)kRingX[i]; q[7] = (uint32_t)kRingY[i];
        const uint32_t m = e->call(0x6F930, 0, game, (uint32_t)(uintptr_t)q);
        e->blk(0x6F930, q, 23);
        if (m) {
            e->call(0xA50E, 0, 0, 0, 0, 0, m, (uint32_t)kRingY[i]);
            e->call(0xAF5E, 0, 0, 0, 0, 0, m, (uint32_t)kRingX[i]);
        }
    }
    return 1;
}
