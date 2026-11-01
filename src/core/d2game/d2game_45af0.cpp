// 1.13c D2Game+0x45AF0 -- area-damage skill with a toggled state (ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = level, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block, M = the bit-mask table.
// The prologue, the state-drop branch and fail (0x8E900(EAX = unit, EBX = skill, ECX = game; 5) then 0) are 0x45850's.
// c = 0xAC9A(unit, record +0x64, skill, level) <= 0: fail. 0x8E900(..; 5), 0x8EA80(game, unit; 5, 0x1C20(skill) + game +0xA8, 0, skill, level).
// No room (0xA478) or 0xA808(room) != 0: 0. A zeroed 28-word damage block: 0x9F1A0(EAX = skill, EBX = unit; block, level),
// 0x9F210(EAX = skill, EDI = unit; block, level); low word of [1] |= record word +0x12E, [0] |= record +0x130;
// 0xA1240(ECX = unit, EDX = block, EAX = record +0x50; game, 0, 0, c). 1.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f45af0(GE& e, uint32_t T, uint32_t M, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
{
    auto fail = [&]() -> uint32_t { e.h(0x8E900, unit, game, 0, sk, 0, 5); return 0; };
    const uint32_t rec = e.skillRec(T, (int32_t)sk);
    if (!rec || (int32_t)lv <= 0) return fail();
    const int32_t st = e.rd16s(rec + 0x80);
    if (st < 0 || st >= (int32_t)e.rd32(T + 0xC4)) return fail();
    if (!e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)st)) return fail();
    if (!(e.rd8(M) & e.rd8(rec + 5))) {
        const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        if (room && e.h(0xA808, 0, 0, 0, 0, 0, room)) {
            const uint32_t sl = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)st);
            if (sl) { e.h(0xABB6, 0, 0, 0, 0, 0, unit, sl); e.h(0xABAA, 0, 0, 0, 0, 0, sl); }
            e.h(0xA3CA, 0, 0, 0, 0, 0, unit, (uint32_t)st, 0);
            return fail();
        }
    }
    const int32_t c = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x64), sk, lv);
    if (c <= 0) return fail();
    const uint32_t fr = e.h(0x1C20, sk) + e.rd32(game + 0xA8);
    e.h(0x8E900, unit, game, 0, sk, 0, 5);
    e.h(0x8EA80, 0, game, unit, 0, 0, 5, fr, 0, sk, lv);
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!room || e.h(0xA808, 0, 0, 0, 0, 0, room)) return 0;
    uint32_t b[28] = {};
    e.h(0x9F1A0, sk, 0, 0, unit, 0, e.stk(b), lv);
    e.h(0x9F210, sk, 0, 0, 0, 0, e.stk(b), lv);
    b[1] = (b[1] & 0xFFFF0000u) | ((b[1] | (e.rd32(rec + 0x12C) >> 16)) & 0xFFFF);
    b[0] |= e.rd32(rec + 0x130);
    e.h(0xA1240, e.rd32(rec + 0x50), unit, e.stk(b), 0, 0, game, 0, 0, (uint32_t)c);
    e.blk(0xA1240, b, 28);
    return 1;
}
}
