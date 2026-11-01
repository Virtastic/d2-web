// 1.13c D2Game+0x456B0 -- fire a skill's missile, then its extra copies (ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = level, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block.
// Skill record out of range: 0. m = 0x91340(EAX = skill; unit); below 0 or at/above the missile count (T +0xB6C): 0. unit +0xC4 |= 0x40.
// n = 0xAC9A(unit, record +0x138, skill, level); n <= 0: 0. 0xA28F0(ECX = game, EDX = m; unit, skill, level, 0, 0, 0, 0, 0). n - 1 <= 0: 1.
// A zeroed 23-word request: [0] = 0x21, [1] = unit, [4] = m, [5]/[6] = the unit's x/y, [11] = skill, [12] = level, [21] = the 0x41F50 callback.
// 0x9FA30(ECX = game, EAX = unit, EBX = &[7]; &[8]) fills the target pair; 0 ends with 1. Else n - 1 times: [22] = i, 0x6F930(ECX = game, EDX = request). 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f456b0(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
{
    const uint32_t rec = e.skillRec(T, (int32_t)sk);
    if (!rec) return 0;
    const int32_t m = (int32_t)e.h(0x91340, sk, 0, 0, 0, 0, unit);
    if (m < 0 || m >= (int32_t)e.rd32(T + 0xB6C)) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    const int32_t n = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x138), sk, lv);
    if (n <= 0) return 0;
    e.h(0xA28F0, 0, game, (uint32_t)m, 0, 0, unit, sk, lv, 0, 0);
    if (n - 1 <= 0) return 1;
    uint32_t b[23] = {};
    b[0] = 0x21; b[1] = unit;
    b[5] = e.unitX(unit);
    b[6] = e.unitY(unit);
    b[4] = (uint32_t)m; b[11] = sk; b[12] = lv; b[21] = 0xB2041F50;
    if (!e.h(0x9FA30, unit, game, 0, e.stk(&b[7]), 0, e.stk(&b[8]))) return 1;
    for (int32_t i = 0; i < n - 1; i++) {
        b[22] = (uint32_t)i;
        e.h(0x6F930, 0, game, e.stk(b));
        e.blk(0x6F930, b, 23);
    }
    return 1;
}
}
