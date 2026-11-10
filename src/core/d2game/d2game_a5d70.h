// 1.13c D2Game+0xA5D70 (AI think callback, ranged/melee mix): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_177e0.h). tp: +8 target, +0x18 mode, +0x1C monstats row. Row words indexed by game byte
// +0x6D: +0x56 / +0x5C chance, +0x62, +0x68 idle frames, +0x170 skill, +0x180 skill byte. The unit seed (+0x20 lo, +0x24 hi) steps new = lo * 0x6AC690C5 + hi.
// Target and mode both set: roll < row +0x56: skill >= 0 and 0x1180 roll(100) (EAX = 100, ECX = &seed) < +0x62 casts 0xF2380 (EAX = target with the low byte
// row +0x180, ECX = unit, EBX = game, stack = skill, target, 0, 0), else sends 0xF2220 (EAX = 4, EDI = ESI = unit, stack = game, target); roll >= +0x56 idles
// 0xF22C0 (EAX = game, ESI = unit, stack = +0x62). Otherwise roll < +0x5C calls 0xF2540 (ECX = game, EDX = unit, stack = target, 2, 0, 0, 1, 0).
// Always ends 0xF20C0 (EAX = game, EBX = unit, stack = +0x68).
#pragma once
#include <cstdint>

namespace ga5d70 {
template <class E> uint32_t Roll(E& e, uint32_t unit)
{
    const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n % 100u;
}
template <class E> void Think(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0xA5D70
    const uint32_t tgt = e.rd32(tp + 8), mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    auto w = [&](uint32_t o) { return e.rd16s(row + o + d); };
    if (tgt && mode) {
        if ((int32_t)Roll(e, unit) < w(0x56)) {
            if (e.rd16s(row + 0x170) >= 0 && (int32_t)e.h(0x1180, 0x64, unit + 0x20) < w(0x62)) {
                e.h(0xF2380, (tgt & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)e.rd16s(row + 0x170), tgt, 0, 0);
            } else {
                e.h(0xF2220, 4, 0, 0, 0, unit, game, tgt); e.j(0xF2220, unit);
            }
        } else {
            e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)w(0x62));
        }
    } else if ((int32_t)Roll(e, unit) < w(0x5C)) {
        e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0, 1, 0);
    }
    e.h(0xF20C0, game, 0, 0, unit, 0, (uint32_t)w(0x68));
}
}
