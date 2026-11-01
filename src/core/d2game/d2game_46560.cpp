// 1.13c D2Game+0x46560 -- summon a pet at a target spot (ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = level, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block.
// Skill record out of range: 0. cls = 0x9EE50(EAX = 0, ECX = 0, EDX = &out; unit, 0, skill, level); below 0: 0. b = record signed byte +0xBE;
// below 0 or at/above the pet-type count (T +0xBF0): 0. unit +0xC4 |= 0x40. 0x9FA30(ECX = game, EAX = unit, EBX = &x; &y) == 0: 0 (x/y unused).
// The 9-word spawn request {0, unit, cls, 0, out, 0, 0, b, 0xAC9A(unit, record +0xC0, skill, level)}: pet = 0xA3610(EAX = request; game); none: 0.
// 0xEFE40(EAX = EDI = pet, ECX = game, EBX = unit +0xD0; 0). c = max(1, 0xAC9A(unit, record +0x13C, skill, level));
// 0x4E2A0(EAX = unit, ECX = c; game, pet, level), 0x4F970(game, unit, pet, skill, level, 0). 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f46560(GE& e, uint32_t T, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
{
    const uint32_t rec = e.skillRec(T, (int32_t)sk);
    if (!rec) return 0;
    uint32_t out = 0;
    const int32_t cls = (int32_t)e.h(0x9EE50, 0, 0, e.stk(&out), 0, 0, unit, 0, sk, lv);
    if (cls < 0) return 0;
    const int32_t b = (int8_t)e.rd8(rec + 0xBE);
    if (b < 0 || b >= (int32_t)e.rd32(T + 0xBF0)) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    uint32_t xy[2] = {};
    if (!e.h(0x9FA30, unit, game, 0, e.stk(&xy[1]), 0, e.stk(&xy[0]))) return 0;
    uint32_t q[9] = { 0, unit, (uint32_t)cls, 0, out, 0, 0, (uint32_t)b, 0 };
    q[8] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0xC0), sk, lv);
    const uint32_t pet = e.h(0xA3610, e.stk(q), 0, 0, 0, 0, game);
    e.blk(0xA3610, q, 9);
    if (!pet) return 0;
    e.h(0xEFE40, pet, game, 0, e.rd32(unit + 0xD0), 0, 0);
    int32_t c = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x13C), sk, lv);
    if (c < 1) c = 1;
    e.h(0x4E2A0, unit, (uint32_t)c, 0, 0, 0, game, pet, lv);
    e.h(0x4F970, 0, 0, 0, 0, 0, game, unit, pet, sk);
    return 1;
}
}
