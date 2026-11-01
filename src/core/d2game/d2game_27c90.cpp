// 1.13c D2Game+0x27C90 -- launch a skill's missile at the unit's target (ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = a, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_gp.h).
// Skill record out of range -> 0. Refresh the target (0xE07A0), resolve unit +0x2C (0xA598); none or itself -> 0. 0xB342(target) zero -> 0. X = 0x1100(target), Y = 0x1130(target);
// 0xA478(target) zero -> 0; 0x1280() zero -> 0; 0xA694(that, local, 2, 0x1000, 1) zero -> 0; target type (word 0) must be 1. 0x1320(target +4) zero -> 0.
// Missile index = record short +0x48, or +0x4C when the 0x1320 result byte +5 meets the mask at G +0xC, or +0x4A when it meets G +8 (G = the pointer at D2Game+0xF8BE0);
// outside 0..table count (table +0xB6C) -> 0. A 23-word request (word 0 = 1, +4 unit, +0x10 missile, +0x14 X, +0x18 Y, +0x2C skill, +0x30 a) goes to 0x6F930(game, request);
// then 0xA3CA(target, 0x68, 1), 0xA3CA(target, 0x76, 1), 0xA4DE(target), 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f27c90(GE& e, uint32_t T, uint32_t G, uint32_t game, uint32_t unit, uint32_t sk, uint32_t a)
{
    const uint32_t srec = e.skillRec(T, (int32_t)sk);
    if (!srec) return 0;
    e.h(0xE07A0, unit);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    if (!e.h(0xB342, 0, 0, 0, 0, 0, tgt)) return 0;
    const uint32_t X = e.h(0x1100, 0, tgt), Y = e.h(0x1130, 0, tgt);
    if (!e.h(0xA478, 0, 0, 0, 0, 0, tgt)) return 0;
    uint32_t loc[4] = {};
    const uint32_t r12 = e.h(0x1280);
    if (!r12) return 0;
    if (!e.h(0xA694, 0, 0, 0, 0, 0, r12, e.stk(loc), 2, 0x1000, 1)) return 0;
    if (e.rd32(tgt) != 1) return 0;
    int32_t m = (int16_t)(e.rd32(srec + 0x48) & 0xFFFF);
    const uint32_t p = e.h(0x1320, e.rd32(tgt + 4));
    if (!p) return 0;
    const uint32_t b = e.rd32(p + 4) >> 8 & 0xFF;
    if (e.rd32(G + 0xC) & b) m = (int16_t)(e.rd32(srec + 0x4C) & 0xFFFF);
    else if (e.rd32(G + 8) & b) m = (int16_t)((e.rd32(srec + 0x48) >> 16) & 0xFFFF);
    if (m < 0 || m >= (int32_t)e.rd32(T + 0xB6C)) return 0;
    uint32_t req[23] = {};
    req[0] = 1; req[1] = unit; req[4] = (uint32_t)m; req[5] = X; req[6] = Y; req[11] = sk; req[12] = a;
    e.h(0x6F930, 0, game, e.stk(req));
    e.h(0xA3CA, 0, 0, 0, 0, 0, tgt, 0x68, 1);
    e.h(0xA3CA, 0, 0, 0, 0, 0, tgt, 0x76, 1);
    e.h(0xA4DE, 0, 0, 0, 0, 0, tgt);
    return 1;
}
}
