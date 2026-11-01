// 1.13c D2Game+0x45EE0 -- attach a timed skill state to a target (stdcall: game, unit, target, duration, skill id, level; ret 0x18).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block.
// Skill record out of range: 0. state = record short +0x82; below 0 or above the state count (a state equal to the count passes): 0.
// expire = game +0xA8 + duration. The target already in the state (0xA5D4): 0. A stat list from 0xA6A0(game +0x1C, 2, expire, target type, target id),
// type 6 / id -1 without a target; none: 0. 0xA5B6(list, state), 0xA75A(list, state callback), 0xA60A(target, list, 1), 0xA3CA(target, state, 1),
// 0xA7C6(list, expire), 0x8EA80(game, target; 0xC, expire, 0, 0, 0), 0x448E0(record; unit, list, skill, level); 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f45ee0(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t tgt, uint32_t dur, uint32_t sk, uint32_t lv)
{
    const uint32_t rec = e.skillRec(T, (int32_t)sk);
    if (!rec) return 0;
    const int32_t st = e.rd16s(rec + 0x82);
    if (st < 0 || st > (int32_t)e.rd32(T + 0xC4)) return 0;
    const uint32_t exp = e.rd32(game + 0xA8) + dur;
    if (e.h(0xA5D4, 0, 0, 0, 0, 0, tgt, (uint32_t)st)) return 0;
    const uint32_t ty = tgt ? e.rd32(tgt) : 6, id = tgt ? e.rd32(tgt + 0xC) : ~0u;
    const uint32_t sl = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 2, exp, ty, id);
    if (!sl) return 0;
    e.h(0xA5B6, 0, 0, 0, 0, 0, sl, (uint32_t)st);
    e.h(0xA75A, 0, 0, 0, 0, 0, sl, 0xB209F7A0);
    e.h(0xA60A, 0, 0, 0, 0, 0, tgt, sl, 1);
    e.h(0xA3CA, 0, 0, 0, 0, 0, tgt, (uint32_t)st, 1);
    e.h(0xA7C6, 0, 0, 0, 0, 0, sl, exp);
    e.h(0x8EA80, 0, game, tgt, 0, 0, 0xC, exp, 0, 0, 0);
    e.h(0x448E0, rec, 0, 0, 0, 0, unit, sl, sk, lv);
    return 1;
}
}
