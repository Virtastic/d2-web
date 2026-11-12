// 1.13c D2Game+0x16E30 (D2GAME_AI_SpecialState13): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_15e00.h). tp: +8 target, +0x14 distance, +0x18 combat flag, +0x1C monstats row (zero returns).
// Needs the unit's room (0xA478) or returns. Home = the unit's ai command of type 10 (ai +0x28 gives ad, ad +0x20 the list, node +8 == 10, +0xC x, +0x10 y); absent:
// a record {?, ?, 10, x, y} is copied in through 0x21290 (EAX = game, ECX = unit) and the list searched again. Zero x or y is re-seeded from the unit. Unit x / y:
// types 2, 4, 5 read unit +0x2C then +0xC / +0x10, others call 0xA454 / 0xA3B8 on unit +0x2C (0 when null). Region = 0xAD66(room, x, y).
// Three run-home gates, each: ai param (ai +0x2C) +0x18 := 1, 0xF2540(game, unit, 0, 0xF, x, y, 1, 0) non-zero returns, else the unit's event list (+0xDC, type 2 nodes,
// +0x24 links) goes through 0x8E6F0 (EAX = node): (1) home region differs from the unit region; (2) home region differs from the target region: at home, distance
// below 0x19 and 0x15F90(tp, game, unit) set returns, else idle 10; away from home the run gate; (3) distance to home (0xB07B0) above 0x28. Then 0x15F90 set returns;
// then combat: roll(100) >= row word +0x62 + 10 idles with row word +0x5C, else mode 4 on the target (0xF2220, EAX = 4); no combat: roll(100) >= row word +0x56 idles
// with +0x5C, else ai param +0x1C := 100 and 0xF2540(game, unit, target, 0xF, 0, 0, 1, 0). Row words index by game byte +0x6D.
#pragma once
#include <cstdint>

namespace g16e30 {
template <class E> uint32_t UX(E& e, uint32_t u)
{
    const uint32_t t = e.rd32(u);
    if (t == 2 || (t > 3 && t <= 5)) return e.rd32(e.rd32(u + 0x2C) + 0xC);
    const uint32_t p = e.rd32(u + 0x2C);
    return p ? e.h(0xA454, 0, 0, 0, 0, 0, p) : 0;
}
template <class E> uint32_t UY(E& e, uint32_t u)
{
    const uint32_t t = e.rd32(u);
    if (t == 2 || (t > 3 && t <= 5)) return e.rd32(e.rd32(u + 0x2C) + 0x10);
    const uint32_t p = e.rd32(u + 0x2C);
    return p ? e.h(0xA3B8, 0, 0, 0, 0, 0, p) : 0;
}
template <class E> uint32_t Home(E& e, uint32_t unit)
{
    if (!unit || e.rd32(unit) != 1) return 0;
    const uint32_t ai = e.rd32(unit + 0x14); if (!ai) return 0;
    const uint32_t ad = e.rd32(ai + 0x28); if (!ad) return 0;
    const uint32_t P = e.rd32(ad + 0x20); if (!P) return 0;
    uint32_t n = e.rd32(P);
    while (e.rd32(n + 8) != 10 && n != P) n = e.rd32(n);
    return e.rd32(n + 8) == 10 ? n : 0;
}
template <class E> uint32_t AiParam(E& e, uint32_t unit)
{
    if (unit && e.rd32(unit) == 1) { const uint32_t ai = e.rd32(unit + 0x14); if (ai) return e.rd32(ai + 0x2C); }
    return 0;
}
template <class E> bool RunHome(E& e, uint32_t game, uint32_t unit, uint32_t cmd)
{
    e.wr32(AiParam(e, unit) + 0x18, 1);
    if (e.h(0xF2540, 0, game, unit, 0, 0, 0, 0xF, e.rd32(cmd + 0xC), e.rd32(cmd + 0x10), 1, 0)) return true;
    for (uint32_t n = e.rd32(unit + 0xDC); n;) { const uint32_t nx = e.rd32(n + 0x24); if (e.rd8(n) == 2) e.h(0x8E6F0, n); n = nx; }
    return false;
}
template <class E> void SpecialState13(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x16E30
    const uint32_t row = e.rd32(tp + 0x1C);
    if (!row) return;
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!room) return;
    const uint32_t tgt = e.rd32(tp + 8);
    const int32_t dist = (int32_t)e.rd32(tp + 0x14);
    const uint32_t combat = e.rd32(tp + 0x18);
    uint32_t cmd = Home(e, unit);
    if (!cmd) {
        uint32_t b[5] = { 0, 0, 10, 0, 0 };
        b[3] = e.h(0x1100, 0, unit);
        b[4] = e.h(0x1130, 0, unit);
        e.h(0x21290, game, unit, 0, 0, 0, e.stk(b));
        e.blk(0x21290, b, 5, 0x1C);
        cmd = Home(e, unit);
    }
    if (!e.rd32(cmd + 0xC) || !e.rd32(cmd + 0x10)) {
        e.wr32(cmd + 0xC, UX(e, unit));
        e.wr32(cmd + 0x10, UY(e, unit));
    }
    const uint32_t hx = e.rd32(cmd + 0xC), hy = e.rd32(cmd + 0x10);
    const uint32_t homeReg = e.h(0xAD66, 0, 0, 0, 0, 0, room, hx, hy);
    {
        const uint32_t y = UY(e, unit), x = UX(e, unit);
        if (homeReg != e.h(0xAD66, 0, 0, 0, 0, 0, room, x, y)) { if (RunHome(e, game, unit, cmd)) return; }
    }
    {
        const uint32_t y = UY(e, tgt), x = UX(e, tgt);
        if (homeReg != e.h(0xAD66, 0, 0, 0, 0, 0, room, x, y)) {
            if (e.h(0x1100, 0, unit) == e.rd32(cmd + 0xC) && e.h(0x1130, 0, unit) == e.rd32(cmd + 0x10)) {
                if (dist >= 0x19 || !e.h(0x15F90, tp, game, 0, 0, 0, unit)) e.h(0xF22C0, game, 0, 0, 0, unit, 10);
                return;
            }
            if (RunHome(e, game, unit, cmd)) return;
        }
    }
    if (e.h(0xB07B0, unit, 0, 0, 0, 0, e.rd32(cmd + 0xC), e.rd32(cmd + 0x10)) > 0x28) { if (RunHome(e, game, unit, cmd)) return; }
    if (e.h(0x15F90, tp, game, 0, 0, 0, unit)) return;
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    if (combat) {
        const int32_t r = (int32_t)e.h(0x1180, 0x64, unit + 0x20);
        if (r >= e.rd16s(row + d + 0x62) + 10) { e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + d + 0x5C)); return; }
        e.h(0xF2220, 4, 0, 0, 0, 0, game, tgt);
        return;
    }
    const int32_t r = (int32_t)e.h(0x1180, 0x64, unit + 0x20);
    if (r >= e.rd16s(row + d + 0x56)) { e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + d + 0x5C)); return; }
    e.wr32(AiParam(e, unit) + 0x1C, 100);
    e.h(0xF2540, 0, game, unit, 0, 0, tgt, 0xF, 0, 0, 1, 0);
}
}
