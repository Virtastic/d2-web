// 1.13c D2Game+0x45CE0 -- item-charged timed state (ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = level, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block.
// Skill record out of range or its state (short +0x80) outside [0, state count): 0. item = 0xAABA(unit); none: 0.
// 0xA49C(item, the 0xFC3A8 source string, 0x48B) != skill: 0. unit +0xC4 |= 0x40. r = 0x1180(EAX = 0x10000, ECX = unit +0x20).
// expire = game +0xA8 + max(1, 0xAC9A(unit, record +0x60, skill, level)). The unit's list for the state (0xA5D4), or one from
// 0xA6A0(game +0x1C, 2, expire, unit type, unit id) (none: 0) set up by 0xA5B6(list, state), 0xA75A(list, callback), 0xA60A(unit, list, 1),
// 0xA3CA(unit, state, 1). Then 0xA7C6(list, expire), 0x8EA80(game, unit; 0xC, expire, 0, 0, 0), 0x448E0(record; unit, list, skill, level),
// 0xA6E2(list, 0x15E, skill, 0), 0xA6E2(list, 0x15F, level, 0), 0x8E900(EAX = unit, EBX = skill, ECX = game; 5),
// 0x8EA80(game, unit; 5, 0x1C20(skill) + game +0xA8, 0, skill, level), 0xAC94(item, r). 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f45ce0(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
{
    const uint32_t rec = e.skillRec(T, (int32_t)sk);
    if (!rec) return 0;
    const int32_t st = e.rd16s(rec + 0x80);
    if (st < 0 || st >= (int32_t)e.rd32(T + 0xC4)) return 0;
    const uint32_t item = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!item) return 0;
    if (e.h(0xA49C, 0, 0, 0, 0, 0, item, 0xB20FC3A8, 0x48B) != sk) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    const uint32_t rnd = e.h(0x1180, 0x10000, unit + 0x20);
    int32_t d = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x60), sk, lv);
    if (d < 1) d = 1;
    const uint32_t exp = e.rd32(game + 0xA8) + (uint32_t)d;
    uint32_t sl = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)st);
    if (!sl) {
        const uint32_t ty = unit ? e.rd32(unit) : 6, id = unit ? e.rd32(unit + 0xC) : ~0u;
        sl = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 2, exp, ty, id);
        if (!sl) return 0;
        e.h(0xA5B6, 0, 0, 0, 0, 0, sl, (uint32_t)st);
        e.h(0xA75A, 0, 0, 0, 0, 0, sl, 0xB209F7A0);
        e.h(0xA60A, 0, 0, 0, 0, 0, unit, sl, 1);
        e.h(0xA3CA, 0, 0, 0, 0, 0, unit, (uint32_t)st, 1);
    }
    e.h(0xA7C6, 0, 0, 0, 0, 0, sl, exp);
    e.h(0x8EA80, 0, game, unit, 0, 0, 0xC, exp, 0, 0, 0);
    e.h(0x448E0, rec, 0, 0, 0, 0, unit, sl, sk, lv);
    e.h(0xA6E2, 0, 0, 0, 0, 0, sl, 0x15E, sk, 0);
    e.h(0xA6E2, 0, 0, 0, 0, 0, sl, 0x15F, lv, 0);
    const uint32_t fr = e.h(0x1C20, sk) + e.rd32(game + 0xA8);
    e.h(0x8E900, unit, game, 0, sk, 0, 5);
    e.h(0x8EA80, 0, game, unit, 0, 0, 5, fr, 0, sk, lv);
    e.h(0xAC94, 0, 0, 0, 0, 0, item, rnd);
    return 1;
}
}
