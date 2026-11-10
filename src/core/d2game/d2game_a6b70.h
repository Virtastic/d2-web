// 1.13c D2Game+0xA6B70 (AI think callback, minion-summon skill with a cast delay): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_177e0.h). tp: +0 the AI control block st (+0x14 ready tick, +0x18 side toggle, +0x1C summoned flag),
// +0x1C the monstats row (short +0x170 = skill). dt = sgptDataTables (skills count +0xBA0, table +0xB98, rows 0x23C; objects count +0xB6C).
// 1. Skill s invalid (s < 0, s >= count, row 0), or the skill object 0xA3F4 (ECX = unit, EDX = s) is 0, or st[+0x14] (set to now := game+0xA8 plus 0xAC9A when negative)
//    is below now: 0xA496(unit, 0) and the command {0, unit, 0, 0, 0, 0x6400, 0, 0} through 0x75E10 (stack = game, &block, 1).
// 2. st[+0x1C] == 0 and the skill row short +0x48 in [0, dt +0xB6C): target t := 0x21260(ECX = unit) or the unit; 0x1130 then 0x1100 (ECX = unit) feed 0xA0000(game, s,
//    A400 result, object, r21100, r21130); a result n: 0xAC76(n, unit), 0xA4D2(unit, 0x13, 0xA7F0(t), 0), a := 0xA42A(t, 0x77, 0) + 0xA80E(t, 0x372C0(t), 0, 0) when 0x372C0 is non-zero,
//    0xA4D2(unit, 0x77, a, 0). st[+0x1C] := 1 in any case.
// 3. A player unit (type 1) with +0x14 +0x28 +0x20 = e: the mode block (+0x14 +0x2C) gets +0x18 := 0xF and byte +0x20 := 0x14; two 0xF2540 walks (ECX = game, EDX = unit, stack 0, 2, x, y, 1, 4)
//    through (e+0xC, e+0x10) then (e+0x14, e+0x18) when st[+0x18] == 0 (and the toggle becomes 1), else the other way round (toggle 0); the first non-zero ends. Both zero:
//    0xF2870 (EAX = (0x21260(unit) & ~0xFF) | 5, ECX = that, stack game, unit); zero again: 0xF22C0 (EAX = game, ESI = unit, stack 5). Not a player unit: 0xF22C0 with 3.
#pragma once
#include <cstdint>

namespace ga6b70 {
template <class E> void Fail(E& e, uint32_t game, uint32_t unit)
{
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t blk[8] = { 0, unit, 0, 0, 0, 0x6400, 0, 0 };
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
    e.blk(0x75E10, blk, 8);
}
template <class E> void Think(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t dt)
{
    // 1.13c D2Game+0xA6B70
    const uint32_t st = e.rd32(tp), mrow = e.rd32(tp + 0x1C);
    const int32_t s = e.rd16s(mrow + 0x170);
    uint32_t rec = 0;
    if (s >= 0 && s < (int32_t)e.rd32(dt + 0xBA0)) rec = (uint32_t)s * 0x23C + e.rd32(dt + 0xB98);
    if (!rec) { Fail(e, game, unit); return; }
    const uint32_t o = e.h(0xA3F4, 0, unit, (uint32_t)s);
    if (!o) { Fail(e, game, unit); return; }
    const uint32_t sk6 = e.h(0xA400, 0, 0, 0, 0, 0, unit, o, 1);
    if ((int32_t)e.rd32(st + 0x14) < 0) {
        const uint32_t d = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0x144), (uint32_t)s, sk6);
        e.wr32(st + 0x14, d + e.rd32(game + 0xA8));
    }
    if ((int32_t)e.rd32(st + 0x14) < (int32_t)e.rd32(game + 0xA8)) { Fail(e, game, unit); return; }
    const int32_t obj = e.rd16s(rec + 0x48);
    if (e.rd32(st + 0x1C) == 0 && obj >= 0 && obj < (int32_t)e.rd32(dt + 0xB6C)) {
        uint32_t t = e.h(0x21260, 0, unit);
        if (!t) t = unit;
        const uint32_t r30 = e.h(0x1130, 0, unit);
        const uint32_t r00 = e.h(0x1100, 0, unit);
        const uint32_t n = e.h(0xA0000, 0, 0, 0, 0, 0, game, (uint32_t)s, sk6, (uint32_t)obj, r00, r30);
        if (n) {
            e.h(0xAC76, 0, 0, 0, 0, 0, n, unit);
            const uint32_t v = e.h(0xA7F0, 0, 0, 0, 0, 0, t);
            e.h(0xA4D2, 0, 0, 0, 0, 0, unit, 0x13, v, 0);
            uint32_t a = e.h(0xA42A, 0, 0, 0, 0, 0, t, 0x77, 0);
            const uint32_t pu = e.h(0x372C0, 0, 0, 0, 0, 0, t);
            if (pu) a += e.h(0xA80E, 0, 0, 0, 0, 0, t, pu, 0, 0);
            e.h(0xA4D2, 0, 0, 0, 0, 0, unit, 0x77, a, 0);
        }
        e.wr32(st + 0x1C, 1);
    }
    uint32_t pl = 0, q = 0, ev = 0;
    if (unit && e.rd32(unit) == 1 && (pl = e.rd32(unit + 0x14)) && (q = e.rd32(pl + 0x28)) && (ev = e.rd32(q + 0x20))) {}
    else { e.h(0xF22C0, game, 0, 0, 0, unit, 3); return; }
    const uint32_t pb = e.rd32(pl + 0x2C);
    e.wr32(pb + 0x18, 0xF); e.wr8(pb + 0x20, 0x14);
    auto walk = [&](uint32_t x, uint32_t y) { return e.h(0xF2540, 0, game, unit, 0, 0, 0, 2, x, y, 1, 4); };
    uint32_t x, y;
    if (e.rd32(st + 0x18) == 0) {
        if (walk(e.rd32(ev + 0xC), e.rd32(ev + 0x10))) return;
        e.wr32(st + 0x18, 1);
        x = e.rd32(ev + 0x14); y = e.rd32(ev + 0x18);
    } else {
        if (walk(e.rd32(ev + 0x14), e.rd32(ev + 0x18))) return;
        e.wr32(st + 0x18, 0);
        x = e.rd32(ev + 0xC); y = e.rd32(ev + 0x10);
    }
    if (walk(x, y)) return;
    const uint32_t r = e.h(0x21260, 0, unit);
    if (!e.h(0xF2870, (r & ~0xFFu) | 5, r, 0, 0, 0, game, unit)) e.h(0xF22C0, game, 0, 0, 0, unit, 5);
}
}
