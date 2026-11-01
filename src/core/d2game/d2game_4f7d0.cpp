// 1.13c D2Game+0x4F7D0: ecx = this, edx = unit, stack a0 = skill id, a1 (ret 8). Returns 0 when the unit's owner (0xA598 of unit+0x2C) is the unit itself,
// null, or fails #A808(#A478(owner)), or when the skill id is outside the skill table. Otherwise builds a 28-word request: word 1 low half = the
// 0xDE5A0 result for (owner, this, level from #ACD6); when its bit 0 is set, word 3 and (when the record's byte +0x1DC is set) word 26 come from
// #AC9A and 0x9F210 is called; then 0xDDDE0 gets the request (edi) with the record's byte +0x1A5 (0x80 when zero) in CL. Returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4f7d0(uint32_t self, uint32_t unit, uint32_t a0, uint32_t a1, uint32_t dt, const D2Seam* e)
{
    e->call(0xE07A0, unit);
    const uint32_t v = e->call(0xA598, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    if (v == unit || v == 0) return 0;
    const uint32_t t = e->call(0xA478, 0, 0, 0, 0, 0, v);
    if (e->call(0xA808, 0, 0, 0, 0, 0, t)) return 0;
    if ((int32_t)a0 < 0 || (int32_t)a0 >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t rec = a0 * 0x23C + d2rd32(dt + 0xB98);
    if (!rec) return 0;
    uint32_t L[28] = {};
    const uint32_t lvl = e->call(0xACD6, 0, 0, 0, 0, 0, unit, a0, a1, 0);
    const uint32_t ax = e->call(0xDE5A0, self, 0, 0, 0, 0, 0, lvl) & 0xFFFF;
    L[1] = ax;
    if (ax & 1) {
        L[3] = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(rec + 0x138), a0, a1);
        if (d2rd8(rec + 0x1DC)) {
            L[26] = e->call(0xAC9A, 0, 0, 0, 0, 0, unit, d2rd32(rec + 0x144), a0, a1);
            if ((int32_t)L[26] > 0) ((uint8_t*)L)[0x65] = (uint8_t)d2rd8(rec + 0x1DC);
            e->call(0x9F210, a0, 0, 0, 0, 0, (uint32_t)(uintptr_t)L, a1);
        }
    }
    uint32_t cl = d2rd8(rec + 0x1A5);
    if (!cl) cl = 0x80;
    e->call(0xDDDE0, unit, cl, 0, 0, 0, self, v);
    e->blk(0xDDDE0, L, 9);
    return 1;
}
