// 1.13c D2Game+0x45850 -- summon-at-a-free-spot skill with a toggled state (ECX = game, EDX = unit, [esp+4] = skill id, [esp+8] = level, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block, M = the bit-mask table.
// fail = 0x8E900(EAX = unit, EBX = skill, ECX = game; 5) then 0. Skill record out of range, level <= 0, the state (record short +0x80) outside
// [0, state count) or 0xA68E(unit, state) == 0: fail. When the mask byte M[0] and record byte +5 share no bit and the unit's room (0xA478) answers
// 0xA808: drop the state (0xA5D4 list; 0xABB6(unit, list), 0xABAA(list) when there is one), 0xA3CA(unit, state, 0), fail.
// m = 0x91340(EAX = skill; unit) <= 0, L = 0xA3F4(ECX = unit, EDX = skill) == 0, c = 0xAC9A(unit, record +0x64, skill, level) <= 0: fail.
// 0x8E900(..; 5), 0x8EA80(game, unit; 5, 0x1C20(skill) + game +0xA8, 0, skill, level). No room or 0xA808(room) != 0: 0 (no fail call).
// Seed: 0xA628(ECX = unit +0x20, EDX = 0xAD12(L)), 0xAC94(L, 0x1180(1000000, unit +0x20)); 0x1100 / 0x1130(ECX = unit) once.
// Five tries: x = 0x1180(2c+1) - c + 0x1100(unit), y = 0x1180(2c+1) - c + 0x1130(unit); 0xA820(x, y, unit, 0x805) != 0 then
// room = 0x1280(EAX = room, EBX = x, EDI = y) (kept for the next try); a room whose 0xAFE8(room, x, y, 1) low word is 0 is the spot:
// 0xA0000(game, skill, level, m, x, y), 0x7A190(skill, level, x, y, 0; EDI = 0), 1. No spot: 0.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f45850(GE& e, uint32_t T, uint32_t M, uint32_t game, uint32_t unit, uint32_t sk, uint32_t lv)
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
    const int32_t m = (int32_t)e.h(0x91340, sk, 0, 0, 0, 0, unit);
    if (m <= 0) return fail();
    const uint32_t L = e.h(0xA3F4, 0, unit, sk);
    if (!L) return fail();
    const int32_t c = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x64), sk, lv);
    if (c <= 0) return fail();
    const uint32_t fr = e.h(0x1C20, sk) + e.rd32(game + 0xA8);
    e.h(0x8E900, unit, game, 0, sk, 0, 5);
    e.h(0x8EA80, 0, game, unit, 0, 0, 5, fr, 0, sk, lv);
    uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!room || e.h(0xA808, 0, 0, 0, 0, 0, room)) return 0;
    const uint32_t seed = unit + 0x20;
    e.h(0xA628, 0, seed, e.h(0xAD12, 0, 0, 0, 0, 0, L));
    e.h(0xAC94, 0, 0, 0, 0, 0, L, e.h(0x1180, 0xF4240, seed));
    e.h(0x1100, 0, unit);
    e.h(0x1130, 0, unit);
    const uint32_t r = 2 * (uint32_t)c + 1;
    for (int i = 0; i < 5; i++) {
        uint32_t x = e.h(0x1180, r, seed) - (uint32_t)c;
        x += e.h(0x1100, 0, unit);
        uint32_t y = e.h(0x1180, r, seed) - (uint32_t)c;
        y += e.h(0x1130, 0, unit);
        if (!e.h(0xA820, 0, 0, 0, 0, 0, x, y, unit, 0x805)) continue;
        room = e.h(0x1280, room, 0, 0, x);
        if (!room) continue;
        if ((e.h(0xAFE8, 0, 0, 0, 0, 0, room, x, y, 1) & 0xFFFF) == 0) {
            e.h(0xA0000, 0, 0, 0, 0, 0, game, sk, lv, (uint32_t)m);
            e.h(0x7A190, 0, 0, 0, 0, 0, sk, lv, x, y);
            return 1;
        }
    }
    return 0;
}
}
