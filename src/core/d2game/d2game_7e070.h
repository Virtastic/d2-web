// 1.13c D2Game+0x7E070 (AI think, hit-and-run flavour; D2MOO baboon block): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +0 ai control (+0x14 phase, +0x18 counter), +8 target, +0x14 distance, +0x18 combat flag,
// +0x1C monstats row. Row words indexed by game byte +0x6D (word per difficulty): P(k) at +0x56+6k; skill +0x170 (word, <0 none), skill byte +0x180, velocity +0x32, run +0x34.
// Rolls: percent rolls are the inline seed step (seed = lo*0x6AC690C5 + hi, result low32 % 100); the skill roll is 0x1180(EAX = 100, ECX = &seed).
#pragma once
#include <cstdint>

namespace g7e070 {
template <class E> uint32_t Step(E& e, uint32_t unit)
{
    const uint64_t t = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5ull + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)t);
    e.wr32(unit + 0x24, (uint32_t)(t >> 32));
    return (uint32_t)t % 100;
}
template <class E> void AiThink(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x7E070
    const uint32_t ai = e.rd32(tp), tgt = e.rd32(tp + 8), comb = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    const int32_t dist = (int32_t)e.rd32(tp + 0x14);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    auto W = [&](uint32_t o) { return e.rd16s(row + o + d); };
    auto idle = [&](uint32_t n) { e.h(0xF22C0, game, 0, 0, 0, unit, n); };
    const int32_t life = (int32_t)e.h(0xA718, 0, 0, 0, 0, 0, unit);
    int32_t nVel = 0;
    const int32_t vel = e.rd16s(row + 0x32);
    if (vel > 0) { nVel = e.rd16s(row + 0x34) * 100 / vel - 100; if (nVel < 1) nVel = 0; else if (nVel > 0x77) nVel = 0x78; }
    const int32_t sk = e.rd16s(row + 0x170);
    auto cast = [&](uint32_t eax) { e.h(0xF2380, (eax & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)sk, tgt, 0, 0); };
    const uint32_t pd = (unit && e.rd32(unit) == 1) ? e.rd32(unit + 0x14) : 0;
    if (pd) {
        const uint32_t st = e.rd32(pd + 0x54);
        if (st == 3 || st == 0x13) {
            e.wr32(ai + 0x14, 0);
            if (comb) {
                e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
                uint32_t flag = 0x64;
                const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
                if (ms && (e.rd8(ms + 0x104) & e.rd32(ga + 0x10))) flag = 0x65;
                uint32_t blk[8] = { 4, unit, tgt, 0, 0, flag << 8, 0, 0 };
                e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
                e.blk(0x75E10, blk, 8);
                return;
            }
            if (sk >= 0 && dist < W(0x6E)) { cast((uint32_t)sk); return; }
        }
    }
    if (e.rd32(ai + 0x14) == 1) {
        e.wr32(ai + 0x18, e.rd32(ai + 0x18) + 1);
        bool clear = W(0x62) < life;
        if (!clear) clear = (int32_t)Step(e, unit) < 0x19;
        if (!clear) clear = (int32_t)e.rd32(ai + 0x18) > W(0x74);
        if (clear) e.wr32(ai + 0x14, 0);
        else if (dist < 14) {
            const uint32_t p2 = pd ? e.rd32(pd + 0x2C) : 0;
            if (nVel != 0) e.wr32(p2 + 0x1C, (uint32_t)nVel);
            e.h(0xF2D80, tgt, 0, 0, 0, 0, game, unit, 0xE, 0);
            return;
        }
        idle(0xF);
        return;
    }
    bool owned = false;
    if (pd) {
        const uint32_t ac = e.rd32(pd + 0x28);
        const uint32_t g2 = ac ? e.rd32(ac + 0x28) : 0;
        if (g2) owned = e.h(0xE03A0, e.rd32(ac + 0x30), g2, e.rd32(ac + 0x2C)) != 0;
    }
    if (!owned && life < W(0x68)) {
        e.wr32(ai + 0x14, 1);
        e.wr32(ai + 0x18, 0);
        e.h(0xF2D80, tgt, 0, 0, 0, 0, game, unit, 9, 0);
        return;
    }
    if (comb) {
        if ((int32_t)Step(e, unit) < W(0x56)) { e.hj(0xF2220, 4, 0, 0, 0, 0, unit, game, tgt); return; }
        if (sk < 0 || (int32_t)e.h(0x1180, 100, unit + 0x20) >= W(0x5C)) { e.h(0xF26B0, 0, unit, 0, 0, 0, game, tgt, 5); return; }
        cast(tgt);
        return;
    }
    if (dist < W(0x6E)) {
        if (sk >= 0 && (int32_t)e.h(0x1180, 100, unit + 0x20) < W(0x5C)) { cast(tgt); return; }
        if (e.h(0xA68E, 0, 0, 0, 0, 0, tgt, 0x54)) e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0);
        else idle((uint32_t)W(0x80));
        return;
    }
    if (dist < W(0x7A)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0); return; }
    idle(0xF);
}
}
