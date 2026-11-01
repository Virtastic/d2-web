// 1.13c D2Game+0x28E70 -- frenzy stat-list refresh (EBX = unit, [esp+4] = game, [esp+8] = skill id, [esp+0xC] = level, ret 0xC).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_gp.h).
// Skill record out of range -> 0. The unit's used skill (0xAABA) must exist and pass 0xAD12, else 0. The record's aura state (signed short at +0x80)
// outside 0..state count -> 1. frame = 0xAC9A(unit, record +0x60, skill, level) + game +0xA8. A stat list for the state (0xA5D4) is reused or made
// (0xA6A0 from game +0x1C, kind 2, frame, unit type/id or 6/-1; none -> 0; then 0xA5B6, 0xA75A, 0xA6E2(list, 0xA9, 0, 0), 0xA60A, 0xA3CA). Then 0xA7C6(list, frame),
// 0x8EA80(game, unit; 0xC, frame, 0, 0, 0), 0xA706, value = 0xA712(list, 0xA9, 0) + 1 capped at level, 0xA6E2(list, 0xA9, value, 0), 0x448E0(record; unit, list, skill, value), 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f28e70(GE& e, uint32_t T, uint32_t unit, uint32_t game, uint32_t sk, uint32_t lv)
{
    const uint32_t srec = e.skillRec(T, (int32_t)sk);
    if (!srec) return 0;
    const uint32_t us = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!us) return 0;
    if (!e.h(0xAD12, 0, 0, 0, 0, 0, us)) return 0;
    const int32_t st = (int16_t)(e.rd32(srec + 0x80) & 0xFFFF);
    if (st < 0 || st >= (int32_t)e.rd32(T + 0xC4)) return 1;
    const uint32_t fr = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(srec + 0x60), sk, lv) + e.rd32(game + 0xA8);
    uint32_t sl = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)st);
    if (!sl) {
        const uint32_t ty = unit ? e.rd32(unit) : 6u, id = unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu;
        sl = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 2, fr, ty, id);
        if (!sl) return 0;
        e.h(0xA5B6, 0, 0, 0, 0, 0, sl, (uint32_t)st);
        e.h(0xA75A, 0, 0, 0, 0, 0, sl, 0xB209F7A0);
        e.h(0xA6E2, 0, 0, 0, 0, 0, sl, 0xA9, 0, 0);
        e.h(0xA60A, 0, 0, 0, 0, 0, unit, sl, 1);
        e.h(0xA3CA, 0, 0, 0, 0, 0, unit, (uint32_t)st, 1);
    }
    e.h(0xA7C6, 0, 0, 0, 0, 0, sl, fr);
    e.h(0x8EA80, 0, game, unit, 0, 0, 0xC, fr, 0, 0, 0);
    e.h(0xA706, 0, 0, 0, 0, 0, unit, (uint32_t)st, 1);
    uint32_t v = e.h(0xA712, 0, 0, 0, 0, 0, sl, 0xA9, 0) + 1;
    if ((int32_t)v > (int32_t)lv) v = lv;
    e.h(0xA6E2, 0, 0, 0, 0, 0, sl, 0xA9, v, 0);
    e.h(0x448E0, srec, 0, 0, 0, 0, unit, sl, sk, v);
    return 1;
}
}
