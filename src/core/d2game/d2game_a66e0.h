// 1.13c D2Game+0xA66E0 (AI think callback, hold position / wander): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly (the decompile misorders the seed stores) as a rule over E (see d2game_177e0.h). tp: +8 target, +0x14 range, +0x18 mode, +0x1C monstats row.
// The unit seed (+0x20 lo, +0x24 hi) steps S' = lo * 0x6AC690C5 + hi (64 bit).
// 1. A unit in mode (+0xD0) 0xB of type 0 or 1 in a game: a 0x10-byte node (Fog alloc 0xA2E6: ECX = [game+0x1C], EDX = 0x10, stack file string 0xFBE1C, 0xA1, 0)
//    {unit, 0, old head, 0} becomes the head at game+0x1118 (old head +0xC = node) and the unit goes to mode 8. An allocation of 0 changes nothing.
// 2. Target set: mode != 0 sends 0xA496(unit, 0), the command {4, unit, target, 0, 0, flag << 8, 0, 0} through 0x75E10 (stack = game, &block, 1), flag 0x65 when the
//    monstats row 0x1320 (EAX = unit +4) byte +0x104 & ga[+0x10] (global 0xF8BE0), else 0x64; then 0xF20C0 (EAX = game, EBX = unit, stack = row +0x56[d]).
//    Mode 0, range < row +0x62[d] and a roll < +0x5C[d]: [ (unit type 1 and unit+0x14: unit+0x14 +0x2C else 0) +0x1C ] := 100, 0xF2540 (ECX = game, EDX = unit,
//    stack = target, 0xF, 0, 0, 1, 0).
// 3. Otherwise (no target, or the gates failed): (X, Y) = unit +0x2C +0xC / +0x10 for unit types 2, 4, 5, else 0xA454 / 0xA3B8 (stack = unit +0x2C) or 0 when +0x2C is 0.
//    Up to three 0xF2540 walks (ECX = game, EDX = unit, stack = 0, 2, x, y, 1, 4), the first non-zero result ends: seed steps S1, S2 (a1 = lo1 % 20, b1 = lo2 % 20):
//    (X + a1 - 0x28, Y + b1 - 10); then S3 (P = lo3 & 1), S4, S5 (a2 = lo4 % 20, b2 = lo5 % 20): (X + a2 - 10, Y + b2 - 10 - 30 * s) with s = P ? 1 : -1; then S6, S7
//    (a3 = lo6 % 20, b3 = lo7 % 20): (X + a3 - 10, Y + b3 - 10 + 30 * s). All three zero: idle 0xF22C0 (EAX = game, ESI = unit, stack = 0xF).
#pragma once
#include <cstdint>

namespace ga66e0 {
template <class E> uint32_t Step(E& e, uint32_t unit)
{
    const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n;
}
template <class E> void Think(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0xA66E0
    const uint32_t tgt = e.rd32(tp + 8), mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    const int32_t range = (int32_t)e.rd32(tp + 0x14);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    const uint32_t type = e.rd32(unit);
    if (e.rd32(unit + 0xD0) == 0xB && game && (type == 0 || type == 1)) {
        const uint32_t old = e.rd32(game + 0x1118);
        const uint32_t n = e.h(0xA2E6, 0, e.rd32(game + 0x1C), 0x10, 0, 0, 0xB20FBE1Cu, 0xA1, 0);
        if (n) {
            e.wr32(n, 0); e.wr32(n + 4, 0); e.wr32(n + 8, 0); e.wr32(n + 12, 0);
            e.wr32(n, unit); e.wr32(n + 4, 0);
            e.wr32(game + 0x1118, n);
            if (old) { e.wr32(n + 8, old); e.wr32(old + 0xC, n); }
            e.wr32(unit + 0xD0, 8);
        }
    }
    if (tgt) {
        if (mode) {
            e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
            uint32_t flag = 0x64;
            const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
            if (ms && (e.rd8(ms + 0x104) & e.rd32(ga + 0x10))) flag = 0x65;
            uint32_t blk[8] = { 4, unit, tgt, 0, 0, flag << 8, 0, 0 };
            e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
            e.blk(0x75E10, blk, 8);
            e.h(0xF20C0, game, 0, 0, unit, 0, (uint32_t)e.rd16s(row + 0x56 + d));
            return;
        }
        if (range < e.rd16s(row + 0x62 + d)) {
            const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
            e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
            if ((int32_t)((uint32_t)n % 100u) < e.rd16s(row + 0x5C + d)) {
                uint32_t q = 0;
                if (type == 1) { const uint32_t p = e.rd32(unit + 0x14); if (p) q = e.rd32(p + 0x2C); }
                e.wr32(q + 0x1C, 100);
                e.h(0xF2540, 0, game, unit, 0, 0, tgt, 0xF, 0, 0, 1, 0);
                return;
            }
        }
    }
    uint32_t X, Y;
    const uint32_t pa = e.rd32(unit + 0x2C);
    if (type == 2 || (type > 3 && type <= 5)) { X = e.rd32(pa + 0xC); Y = e.rd32(pa + 0x10); }
    else {
        X = pa ? e.h(0xA454, 0, 0, 0, 0, 0, pa) : 0;
        Y = pa ? e.h(0xA3B8, 0, 0, 0, 0, 0, pa) : 0;
    }
    auto walk = [&](uint32_t x, uint32_t y) { return e.h(0xF2540, 0, game, unit, 0, 0, 0, 2, x, y, 1, 4); };
    uint32_t l = Step(e, unit);
    const uint32_t x1 = l % 20 + X - 0x28;
    l = Step(e, unit);
    if (walk(x1, l % 20 + Y - 10)) return;
    const uint32_t l3 = Step(e, unit);
    const int32_t s = (l3 & 1) ? 1 : -1;
    l = Step(e, unit);
    const uint32_t x2 = l % 20 + X - 10;
    l = Step(e, unit);
    if (walk(x2, l % 20 - (uint32_t)(s * 30) + Y - 10)) return;
    l = Step(e, unit);
    const uint32_t x3 = l % 20 + X - 10;
    l = Step(e, unit);
    if (walk(x3, l % 20 - (uint32_t)(-s * 30) + Y - 10)) return;
    e.h(0xF22C0, game, 0, 0, 0, unit, 0xF);
}
}
