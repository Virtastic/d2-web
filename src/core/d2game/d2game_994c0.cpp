// 1.13c D2Game+0x994C0 -- Blessed Hammer cast: ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = level (ret 8). Written from the 1.13c disassembly
// as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block.
// Skill id below 0, at/above the skills count (T +0xBA0) or a null record (T +0xB98 + id * 0x23C): 0. mid = 0x91340(EAX = skill; unit): <= 0: 0.
// 0x9FA30(EAX = unit, ECX = game, EBX = &x; &y) == 0: 0. unit +0xC4 |= 0x40. The 23-word missile request {0x20, unit, unit, 0, mid, ..., x, y, ..., skill, level}
// (words 0, 1, 2, 4, 7, 8, 11, 12) goes to 0x6F930(ECX = game, EDX = request): none: 0. With d = missile +0x2C: 0xAAFC(d, 0xE), 0xAAF0(d, missile, 0).
// bonus = 0xAF76(unit, skill); nonzero: for stats 0x34 and 0x35, v = 0xA3E2(missile, stat, 0), v = 0x14D0(EAX = bonus + 100, ECX = 100, EDX = v),
// 0xA472(missile, stat, v, 0). 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f994c0(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
{
    if ((int32_t)sk < 0 || (int32_t)sk >= (int32_t)e.rd32(T + 0xBA0)) return 0;
    if (e.rd32(T + 0xB98) + sk * 0x23Cu == 0) return 0;
    const int32_t mid = (int32_t)e.h(0x91340, sk, 0, 0, 0, 0, unit);
    if (mid <= 0) return 0;
    uint32_t xy[2] = {};
    if (!e.h(0x9FA30, unit, game, 0, e.stk(&xy[0]), 0, e.stk(&xy[1]))) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    uint32_t q[23] = { 0x20, unit, unit, 0, (uint32_t)mid, 0, 0, xy[0], xy[1], 0, 0, sk, lv };
    const uint32_t m = e.h(0x6F930, 0, game, e.stk(q));
    e.blk(0x6F930, q, 23);
    if (!m) return 0;
    const uint32_t d = e.rd32(m + 0x2C);
    e.h(0xAAFC, 0, 0, 0, 0, 0, d, 0xE);
    e.h(0xAAF0, 0, 0, 0, 0, 0, d, m, 0);
    const uint32_t bonus = e.h(0xAF76, 0, 0, 0, 0, 0, unit, sk);
    if (bonus) {
        const uint32_t f = bonus + 0x64;
        for (uint32_t st = 0x34; st <= 0x35; st++) {
            uint32_t v = e.h(0xA3E2, 0, 0, 0, 0, 0, m, st, 0);
            v = e.h(0x14D0, f, 0x64, v);
            e.h(0xA472, 0, 0, 0, 0, 0, m, st, v, 0);
        }
    }
    return 1;
}
}
