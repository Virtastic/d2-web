// 1.13c D2Game+0x19080 (AITHINK_Fn085_HighPriest): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_17cb0.h). tp: C = tp[0] (C+0x14 phase, C+0x18 next-skill tick), T = tp[2] target,
// d = tp[5] distance, f = tp[6] flag, row = tp[7] (monstats AI params). W(o) = short at row + o + 2 * game byte 0x6D. tick = game+0xA8. seed = unit+0x20 (one
// step: n = lo * 0x6AC690C5 + hi; lo' = low, hi' = high; roll = lo' % 100). 0x1180 (EAX = 100, ECX = &seed) is the library roll, a hook here.
// phase 0, f != 0: roll < W(0x56) sets phase 1 and submits a queued command of kind 4 (below), else 0xF2D80 (EAX = T, game, unit, 6, 1).
// phase 0, f == 0: (1) row short 0x172 >= 0 and tick > C+0x18 and 0x1180 < W(0x5c): 0xB17B0 (EAX = 1, ECX = &{0, 0x4B, 0x9C4}, EDX = game, unit, 0xB2000000|0x15660)
// fills local[0]; nonzero: C+0x18 = W(0x62) + tick, 0xF2380 (AL = row byte 0x181 over the sign-extended short 0x172, ECX = unit, EBX = game, id, local0, 0, 0), ends.
// (2) short 0x170 >= 0 and tick > C+0x18 and d < W(0x80) and 0x1180 < W(0x68): a = 0x1450(4), x = 0x1100(ECX = T) + dir[a].x, y = 0x1130(ECX = T) + dir[a].y (dir = the
// 8-byte table at RVA 0xFD038), 0xF2380 (AL = byte 0x180 over the short 0x170, ECX = unit, EBX = game, id, T, x, y), C+0x18 = tick + 0x64, ends. (3) W(0x6E) > 0 and short 0x3E > 0
// and 0x1250(EAX = it) != 0 and d < short(rec+0x96) - 2 and 0x1180 < W(0x6E): 0xF2220 (EAX = 8, game, T), ends. Else a roll < 0x50: d > W(0x80) 0xF2540 (ECX = game, EDX = unit,
// T, 2, 0, 0, 6, 0) else 0xF26B0 (EAX = 0, ECX = unit, game, T, 3), ends; otherwise falls to phase 1 with phase := 1.
// phase 1 (phase != 0 enters here with phase := 1), f != 0: roll < W(0x7A) submits kind 8; else roll2 < W(0x74): 0xF2D80 (.., 6, 1) and phase := 0; else 0x1180 < 0x5A 0xF2220 (EAX = 4,
// game, T) else idle 0xF22C0 (EAX = game, ESI = unit, 0xA). f == 0: d < 6 and roll < W(0x7A): 0xF2220 (EAX = 8, game, T); else roll >= W(0x74) then roll2 < 0x46 0xF2540 (.., T, 2, 0, 0, 1, 7) else
// 0xF1B10 (EAX = 0xC, game, unit); roll < W(0x74): phase := 0, idle.
// Submit kind k (4 or 8): 0xA496 (unit, 0), then 0x75E10 (game, &blk, 1) with blk = {k, unit, T, 0, 0, byte5 = 0x64 or 0x65, 0, 0}; 0x65 when unit and 0x1320 (EAX = unit class)
// returns a record whose byte +0x104 (kind 4) / +0x105 (kind 8) ANDs nonzero with the bit mask word at bm+0x10 (kind 4) / bm+0 (kind 8).
#pragma once
#include <cstdint>

namespace g19080 {
template <class E> void Think(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t dir, uint32_t bm)
{
    // 1.13c D2Game+0x19080
    const uint32_t C = e.rd32(tp), T = e.rd32(tp + 8), f = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    const int32_t d = (int32_t)e.rd32(tp + 0x14);
    const uint32_t S = unit + 0x20, b = e.rd8(game + 0x6D);
    auto W = [&](uint32_t o) { return (int32_t)e.rd16s(row + o + b * 2); };
    auto step = [&]() { const uint64_t v = (uint64_t)e.rd32(S) * 0x6AC690C5u + e.rd32(S + 4); e.wr32(S + 4, (uint32_t)(v >> 32)); e.wr32(S, (uint32_t)v); return (int32_t)((uint32_t)v % 100); };
    auto lib = [&]() { return (int32_t)e.h(0x1180, 100, S); };
    auto idle = [&]() { e.h(0xF22C0, game, 0, 0, 0, unit, 0xA); };
    auto f2220 = [&](uint32_t a) { e.h(0xF2220, a, 0, 0, 0, 0, game, T); };
    auto submit = [&](uint32_t k) {
        e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
        uint32_t blk[8] = { k, unit, T, 0, 0, 0, 0, 0 };
        uint32_t bl = 0x64;
        if (unit) {
            const uint32_t rec = e.h(0x1320, e.rd32(unit + 4));
            if (rec && (k == 4 ? (e.rd8(rec + 0x104) & e.rd32(bm + 0x10)) : (e.rd8(rec + 0x105) & e.rd32(bm)))) bl = 0x65;
        }
        blk[5] = bl << 8;
        e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
        e.blk(0x75E10, blk, 8);
    };
    if (e.rd32(C + 0x14) == 0) {
        if (f) {
            if (step() >= W(0x56)) { e.h(0xF2D80, T, 0, 0, 0, 0, game, unit, 6, 1); return; }
            e.wr32(C + 0x14, 1);
            submit(4);
            return;
        }
        const int32_t s172 = (int32_t)e.rd16s(row + 0x172), s170 = (int32_t)e.rd16s(row + 0x170);
        if (s172 >= 0 && (int32_t)e.rd32(game + 0xA8) > (int32_t)e.rd32(C + 0x18) && lib() < W(0x5C)) {
            uint32_t loc[3] = { 0, 0x4B, 0x9C4 };
            e.h(0xB17B0, 1, e.stk(loc), game, 0, 0, unit, 0xB2000000u | 0x15660);
            if (loc[0]) {
                e.wr32(C + 0x18, (uint32_t)(W(0x62) + (int32_t)e.rd32(game + 0xA8)));
                e.h(0xF2380, ((uint32_t)s172 & ~0xFFu) | e.rd8(row + 0x181), unit, 0, game, 0, (uint32_t)s172, loc[0], 0, 0);
                return;
            }
        }
        if (s170 >= 0 && (int32_t)e.rd32(game + 0xA8) > (int32_t)e.rd32(C + 0x18) && d < W(0x80) && lib() < W(0x68)) {
            const uint32_t a = e.h(0x1450, 0, 0, 0, 0, 0, 4);
            const uint32_t x = e.h(0x1100, 0, T) + e.rd32(dir + a * 8);
            const uint32_t y = e.h(0x1130, 0, T) + e.rd32(dir + a * 8 + 4);
            e.h(0xF2380, ((uint32_t)s170 & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)s170, T, x, y);
            e.wr32(C + 0x18, e.rd32(game + 0xA8) + 0x64);
            return;
        }
        const int32_t s3e = (int32_t)e.rd16s(row + 0x3E);
        if (W(0x6E) > 0 && s3e > 0) {
            const uint32_t mr = e.h(0x1250, (uint32_t)s3e);
            if (mr && d < (int32_t)e.rd16s(mr + 0x96) - 2 && lib() < W(0x6E)) { f2220(8); return; }
        }
        if (step() < 0x50) {
            if (d > W(0x80)) e.h(0xF2540, 0, game, unit, 0, 0, T, 2, 0, 0, 6, 0);
            else e.h(0xF26B0, 0, unit, 0, 0, 0, game, T, 3);
            return;
        }
    }
    e.wr32(C + 0x14, 1);
    if (f) {
        if (step() < W(0x7A)) { submit(8); return; }
        if (step() < W(0x74)) { e.h(0xF2D80, T, 0, 0, 0, 0, game, unit, 6, 1); e.wr32(C + 0x14, 0); return; }
        if (lib() < 0x5A) { f2220(4); return; }
        idle(); return;
    }
    if (d < 6 && step() < W(0x7A)) { f2220(8); return; }
    if (step() < W(0x74)) { e.wr32(C + 0x14, 0); idle(); return; }
    if (step() < 0x46) e.h(0xF2540, 0, game, unit, 0, 0, T, 2, 0, 0, 1, 7);
    else e.h(0xF1B10, 0xC, 0, 0, 0, 0, game, unit);
}
}
