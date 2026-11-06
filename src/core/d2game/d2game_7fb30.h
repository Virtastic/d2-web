// 1.13c D2Game+0x7FB30 (AI think, melee charger; D2MOO CorruptLancer block): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_7e070.h). tp: +0 ai control (+0x14 phase flag), +8 target, +0x14 distance, +0x18 combat flag,
// +0x1C monstats row, +0x20 monstats2 row (+0xE melee range = step count). Row words indexed by game byte +0x6D: P(k) at +0x56+6k; skill words +0x170, skill bytes +0x180.
// Rolls: percent rolls are the inline seed step; the skill roll is 0x1180(EAX = 100, ECX = &seed). Steps byte 0 is raised to 1 by the run callees.
#pragma once
#include <cstdint>

namespace g7fb30 {
template <class E> uint32_t Step(E& e, uint32_t unit)
{
    const uint64_t t = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5ull + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)t);
    e.wr32(unit + 0x24, (uint32_t)(t >> 32));
    return (uint32_t)t % 100;
}
template <class E> void AiThink(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x7FB30
    const uint32_t ai = e.rd32(tp), tgt = e.rd32(tp + 8), comb = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C), m2 = e.rd32(tp + 0x20);
    const int32_t dist = (int32_t)e.rd32(tp + 0x14);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    auto W = [&](uint32_t o) { return e.rd16s(row + o + d); };
    auto idle = [&](uint32_t n) { e.h(0xF22C0, game, 0, 0, 0, unit, n); };
    const uint32_t steps = e.rd8(m2 + 0xE);
    auto vel = [&]() {
        const uint32_t pd = (unit && e.rd32(unit) == 1) ? e.rd32(unit + 0x14) : 0;
        const uint32_t p2 = pd ? e.rd32(pd + 0x2C) : 0;
        e.wr32(p2 + 0x18, 0xD);
        e.wr32(p2 + 0x1C, 100);
    };
    auto setStep = [&]() { e.h(0xAD18, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C), steps); };
    if (dist > W(0x6E)) {
        setStep();
        vel();
        e.h(0xF2540, 0, game, unit, 0, 0, tgt, 0xF, 0, 0, steps ? steps : 1, 0);
        e.wr32(ai + 0x14, 1);
        return;
    }
    if (!comb) {
        if ((int32_t)Step(e, unit) >= W(0x56)) { idle((uint32_t)W(0x62)); return; }
        setStep();
        if ((int32_t)Step(e, unit) >= W(0x68)) { e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0, 3, 0); return; }
        vel();
        e.h(0xF2F00, 0, tgt, unit, 0, 0, game, steps);
        return;
    }
    if (e.rd32(ai + 0x14) == 0 && (int32_t)Step(e, unit) >= W(0x5C)) { idle((uint32_t)W(0x62)); return; }
    e.wr32(ai + 0x14, 0);
    for (uint32_t k = 0; k < 3; ++k) {
        const int32_t sk = e.rd16s(row + 0x170 + 2 * k);
        if (sk >= 0 && (int32_t)e.h(0x1180, 100, unit + 0x20) < W(0x74 + 6 * k)) {
            const uint32_t eax = (k == 1 ? (uint32_t)sk : tgt);
            e.h(0xF2380, (eax & ~0xFFu) | e.rd8(row + 0x180 + k), unit, 0, game, 0, (uint32_t)sk, tgt, 0, 0);
            return;
        }
    }
    e.hj(0xF2220, 4, 0, 0, 0, 0, unit, game, tgt);
}
}
