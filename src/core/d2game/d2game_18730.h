// 1.13c D2Game+0x18730 (AITHINK_Fn115_SiegeBeast): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +8 target, +0x14 range, +0x18 mode, +0x1C monstats row.
// Owner: a unit with flag bit 10 of +0xC8 asks 0xE03A0 (ECX = game, EDX = unit +0x98, EAX = unit +0x94), else none. 0xA688(unit) != 0 ends the tick.
// No owner and 0xA514(unit) == 0: 0xB17B0 (EAX = 1, ECX = &{residue, w56 * w56}, EDX = game, stack = unit, 0x6FC35B00); a hit goes to 0x158F0
// (ESI = hit, stack = unit). The unit stays the actor afterwards (ESI is reloaded from the frame).
// Mode != 0: skill >= 0 and roll < w62 casts 0xF2380 (EAX = skill byte +0x180, ECX = unit, EBX = game, stack = skill, 0, 0, 0); roll < w5C walks with
// 0xF2220 (EAX = 4, EDI = ESI = unit, stack = game, target); else idle 0xF22C0 (EAX = game, ESI = unit, stack = w68).
// Mode 0: skill >= 0 and range < 0x1C50(EAX = skill) and 0x1180(EAX = 100, ECX = &seed) < w6E casts (EAX = sext skill with low byte +0x180).
// Else 0xB0E90(EAX = target, ECX = unit) != 0 and roll < w74: v = w7A (<= 0 is 0, >= 127 is the fatal exit) goes to [[unit+0x14]+0x2C]+0x1C for a
// type-1 unit with data; then 0xF1D90 (EAX = 0xC, ECX = target, stack = game, unit, 2, 0). wNN = row short +NN indexed by the game byte +0x6D.
#pragma once
#include <cstdint>

namespace g18730 {
template <class E> uint32_t Roll(E& e, uint32_t unit)
{
    const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n % 100u;
}
template <class E> void SiegeBeast(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x18730
    const uint32_t range = e.rd32(tp + 0x14), mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C), tgt = e.rd32(tp + 8);
    uint32_t owner = 0;
    if (unit && (e.rd32(unit + 0xC8) >> 10 & 1)) owner = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
    if (e.h(0xA688, 0, 0, 0, 0, 0, unit)) return;
    if (!owner && !e.h(0xA514, 0, 0, 0, 0, 0, unit)) {
        const int32_t v = e.rd16s(row + 0x56 + e.rd8(game + 0x6D) * 2);
        uint32_t blk[2] = { 0, (uint32_t)(v * v) };
        const uint32_t hit = e.h(0xB17B0, 1, e.stk(blk), game, 0, 0, unit, 0x6FC35B00u);
        e.blk(0xB17B0, blk, 2, 0x2);
        if (hit) e.h(0x158F0, 0, 0, 0, 0, hit, unit);
    }
    const int32_t sk = e.rd16s(row + 0x170);
    if (mode) {
        if (sk >= 0 && (int32_t)Roll(e, unit) < e.rd16s(row + 0x62 + e.rd8(game + 0x6D) * 2)) {
            e.h(0xF2380, e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)sk, 0, 0, 0);
            return;
        }
        if ((int32_t)Roll(e, unit) < e.rd16s(row + 0x5C + e.rd8(game + 0x6D) * 2)) { e.h(0xF2220, 4, 0, 0, 0, unit, game, tgt); return; }
        e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)e.rd16s(row + 0x68 + e.rd8(game + 0x6D) * 2));
        return;
    }
    if (sk >= 0 && (int32_t)range < (int32_t)e.h(0x1C50, (uint32_t)sk)) {
        const uint32_t r = e.h(0x1180, 100, unit + 0x20);
        if ((int32_t)r < e.rd16s(row + 0x6E + e.rd8(game + 0x6D) * 2)) {
            e.h(0xF2380, ((uint32_t)sk & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)sk, 0, 0, 0);
            return;
        }
    }
    if (e.h(0xB0E90, tgt, unit) && (int32_t)Roll(e, unit) < e.rd16s(row + 0x74 + e.rd8(game + 0x6D) * 2)) {
        int32_t v = e.rd16s(row + 0x7A + e.rd8(game + 0x6D) * 2);
        if (v <= 0) v = 0;
        // v >= 127: retail's fatal exit (0xA2DA(0x110), 0xA2C2, 0x2A3C(-1)); the descriptor never drives it
        uint32_t ct = 0;
        if (unit && e.rd32(unit) == 1 && e.rd32(unit + 0x14)) ct = e.rd32(e.rd32(unit + 0x14) + 0x2C);
        if (v) e.wr32(ct + 0x1C, (uint32_t)v);
    }
    e.h(0xF1D90, 0xC, tgt, 0, 0, 0, game, unit, 2, 0);
}
}
