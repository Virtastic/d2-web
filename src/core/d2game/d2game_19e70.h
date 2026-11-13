// 1.13c D2Game+0x19E70 (monster AI tick with a skill-list node, a state-walk and a queued command tail): ECX = pGame, EDX = pUnit, stack = pTickParam (ret 4; the
// result is residue). Written from the 1.13c disassembly as a rule over E (see d2game_19080.h); every callee is a hook. Q: C = Q[0] (C+0x14 counter, C+0x18 count,
// C+0x1C flag), T = Q[2] target, d = Q[5] distance (signed, mutable), f = Q[6] flag, row = Q[7] (monstats AI params). b = game byte 0x6D. seed = unit+0x20 (one step:
// n = lo * 0x6AC690C5 + hi; lo' = low, hi' = high; roll = lo' % 100). ai = unit type 1 and unit+0x14 ? [[unit+0x14]+0x2C] : 0.
// node: unit type 1, a = [unit+0x14], b = [a+0x28], h = [b+0x20]: s = [h]; while ([s+8] != 10 and s != h) s = [s]; node = [s+8] == 10 ? s : 0. No node: 0x21290 (EAX = game,
// ECX = unit, block {?, ?, 10, v34, v38, ...} of which only words 2-4 are written; v34/v38 = [[unit+0x2C]+0xC / +0x10] for type 2, 4, 5, else A454/A3B8 of unit+0x2C or 0),
// then the search again. node: R = 0xB0980 (EBX = T; node+0xC, node+0x10); d > 0x2D: idle 0xF22C0 (EAX = game, ESI = unit, 5), ends. R < 0x32 (signed): r2 = 0xB0980 (EBX =
// unit) <= 0x32 (unsigned) skips the next step. Else C[0x1C] = 1, ai[0x18] = 7, ai[0x1C] = 0x64, 0xF2540 (ECX = game, EDX = unit, 0, 0xF, n0, n1, 1, 0) nonzero ends, else the
// unit+0xDC node walk calls 0x8E6F0 (EAX = node) on byte-type-2 nodes. Then C[0x1C] != 0 and 0xB0980 (EBX = unit) > 5 (unsigned) repeats that step; C[0x1C] = 0 after.
// d > 0x14 and R < 0x32: d = max(d >> 1, 0xC), ai writes, 0xF2750 (AL = d, ECX = T, game, unit) nonzero ends else the walk. C[0x14] += 3. short row[0x170] >= 0, f == 0,
// C[0x18] < 2b + 8 and 0x1180(100, seed) < C[0x14]: A = 0x1180(0xF) + 5; 0x1450(ESI = seed, 2) picks which of dx, dy is A and which is the 0x11D0 (ECX = seed, ESI = A)
// roll, two more 0x1450 calls negate dx then dy, 0xF2380 (AL = row byte 0x180, ECX = unit, EBX = game, short 0x170, 0, 1100(T) + dx, 1130(T) + dy), C[0x14] = 0, C[0x18]++, ends.
// d > 5: roll < 5 and R < 0x32 runs ai writes and 0xF2750 (AL = 0xC), ends. Else R2 = 0xB1BC0 (EAX = unit, ECX = game, EBX = &local, &local); R2 != 0 and 0xB0BE0 (EAX = unit)
// == 0 and 0x1180(100) < 0x50: short row[0x172] >= 0 and 0x1180(100) < (b + 4) * 10 runs 0xF2380 (AL = row byte 0x181, short 0x172, R2, 0, 0), else 0xF2220 (EAX = 4, EDI = unit,
// game, R2); ends. Else ai[0x1C] = 0x32, ai[0x18] = 5 or 6 (a seed step, low byte < 0x80 gives 5), byte ai[0x20] = 4, 0xF2540 (.., T, 2, 0, 0, 1, 4) nonzero ends. Then (d <= 5 enters
// here): roll < 0x1E and d < 0xC (signed) runs ai writes and 0xF2C10 (EAX = T, EBX = unit, game, (0xC - d) & 0xFF, 0) nonzero ends else the walk. Tail: 0xA496 (unit, 0), then 0x75E10
// (game, &blk, 1) with blk = {4, unit, T, 0, 0, byte5 = 0x64 or 0x65, 0, 0}; 0x65 when 0x1320 (EAX = unit class) returns a record whose byte +0x104 ANDs nonzero with bm[0x10].
#pragma once
#include <cstdint>

namespace g19e70 {
template <class E> void Think(E& e, uint32_t game, uint32_t unit, uint32_t Q, uint32_t bm)
{
    // 1.13c D2Game+0x19E70
    const uint32_t C = e.rd32(Q), T = e.rd32(Q + 8), f = e.rd32(Q + 0x18), row = e.rd32(Q + 0x1C);
    int32_t d = (int32_t)e.rd32(Q + 0x14);
    const uint32_t S = unit + 0x20, b = e.rd8(game + 0x6D);
    uint32_t lastlo = 0;
    auto step = [&]() { const uint64_t v = (uint64_t)e.rd32(S) * 0x6AC690C5u + e.rd32(S + 4); e.wr32(S + 4, (uint32_t)(v >> 32)); e.wr32(S, (uint32_t)v); lastlo = (uint32_t)v; return (int32_t)((uint32_t)v % 100); };
    auto lib = [&](uint32_t n) { return (int32_t)e.h(0x1180, n, S); };
    auto aiOf = [&]() -> uint32_t { if (e.rd32(unit) == 1) { const uint32_t a = e.rd32(unit + 0x14); if (a) return e.rd32(a + 0x2C); } return 0; };
    auto aiw = [&]() { const uint32_t p = aiOf(); e.wr32(p + 0x18, 7); e.wr32(p + 0x1C, 0x64); };
    auto walk = [&]() { for (uint32_t n = e.rd32(unit + 0xDC); n;) { const uint32_t nx = e.rd32(n + 0x24); if (e.rd8(n) == 2) e.h(0x8E6F0, n); n = nx; } };
    auto findNode = [&]() -> uint32_t {
        if (!unit || e.rd32(unit) != 1) return 0;
        const uint32_t a = e.rd32(unit + 0x14); if (!a) return 0;
        const uint32_t bb = e.rd32(a + 0x28); if (!bb) return 0;
        const uint32_t h = e.rd32(bb + 0x20); if (!h) return 0;
        uint32_t s = e.rd32(h);
        while (e.rd32(s + 8) != 10 && s != h) s = e.rd32(s);
        return e.rd32(s + 8) == 10 ? s : 0;
    };
    uint32_t node = findNode();
    if (!node) {
        const int32_t t = (int32_t)e.rd32(unit);
        const uint32_t p = e.rd32(unit + 0x2C);
        uint32_t v34, v38;
        if (t == 2 || (t > 3 && t <= 5)) { v34 = e.rd32(p + 0xC); v38 = e.rd32(p + 0x10); }
        else { v34 = p ? e.h(0xA454, 0, 0, 0, 0, 0, p) : 0; v38 = p ? e.h(0xA3B8, 0, 0, 0, 0, 0, p) : 0; }
        uint32_t blk[8] = { 0, 0, 10, v34, v38, 0, 0, 0 };
        e.h(0x21290, game, unit, 0, 0, 0, e.stk(blk));
        e.blk(0x21290, blk, 8, 0x1C);
        node = findNode();
    }
    uint32_t R = 0;
    if (node) {
        const uint32_t n0 = e.rd32(node + 0xC), n1 = e.rd32(node + 0x10);
        R = e.h(0xB0980, 0, 0, 0, T, 0, n0, n1);
        if (d > 0x2D) { e.h(0xF22C0, game, 0, 0, 0, unit, 5); return; }
        bool go = (int32_t)R >= 0x32;
        if (!go) go = e.h(0xB0980, 0, 0, 0, unit, 0, n0, n1) > 0x32;
        if (go) {
            e.wr32(C + 0x1C, 1);
            aiw();
            if (e.h(0xF2540, 0, game, unit, 0, 0, 0, 0xF, n0, n1, 1, 0)) return;
            walk();
        }
        if (e.rd32(C + 0x1C) && e.h(0xB0980, 0, 0, 0, unit, 0, n0, n1) > 5) {
            aiw();
            if (e.h(0xF2540, 0, game, unit, 0, 0, 0, 0xF, n0, n1, 1, 0)) return;
            walk();
        }
        e.wr32(C + 0x1C, 0);
    }
    if (d > 0x14 && (int32_t)R < 0x32) {
        d >>= 1; if (d < 0xC) d = 0xC;
        aiw();
        if (e.h(0xF2750, (uint32_t)d & 0xFF, T, 0, 0, 0, game, unit)) return;
        walk();
    }
    e.wr32(C + 0x14, e.rd32(C + 0x14) + 3);
    if (e.rd16s(row + 0x170) >= 0 && f == 0 && (int32_t)e.rd32(C + 0x18) < (int32_t)(b * 2 + 8) && lib(100) < (int32_t)e.rd32(C + 0x14)) {
        const int32_t A = lib(0xF) + 5;
        int32_t dx, dy;
        if (e.h(0x1450, 0, 0, 0, 0, S, 2)) { dx = A; dy = (int32_t)e.h(0x11D0, 0, S, 0, 0, (uint32_t)A); }
        else { dx = (int32_t)e.h(0x11D0, 0, S, 0, 0, (uint32_t)A); dy = A; }
        if (e.h(0x1450, 0, 0, 0, 0, S, 2)) dx = -dx;
        if (e.h(0x1450, 0, 0, 0, 0, S, 2)) dy = -dy;
        const uint32_t y = e.h(0x1130, 0, T) + (uint32_t)dy, x = e.h(0x1100, 0, T) + (uint32_t)dx;
        e.h(0xF2380, e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)e.rd16s(row + 0x170), 0, x, y);
        e.wr32(C + 0x14, 0);
        e.wr32(C + 0x18, e.rd32(C + 0x18) + 1);
        return;
    }
    if (d > 5) {
        if (step() < 5 && (int32_t)R < 0x32) { aiw(); e.h(0xF2750, 0xC, T, 0, 0, 0, game, unit); return; }
        uint32_t l1 = 0, l2 = 0;
        const uint32_t r2 = e.h(0xB1BC0, unit, game, 0, e.stk(&l1), 0, e.stk(&l2));
        if (r2 && !e.h(0xB0BE0, unit) && lib(100) < 0x50) {
            const int32_t lim = (int32_t)((b + 4) * 10);
            if (e.rd16s(row + 0x172) >= 0 && lib(100) < lim) { e.h(0xF2380, e.rd8(row + 0x181), unit, 0, game, 0, (uint32_t)e.rd16s(row + 0x172), r2, 0, 0); return; }
            e.h(0xF2220, 4, 0, 0, 0, 0, game, r2); e.j(0xF2220, unit);
            return;
        }
        e.wr32(aiOf() + 0x1C, 0x32);
        step();
        e.wr32(aiOf() + 0x18, (lastlo & 0xFF) < 0x80 ? 5 : 6);
        e.wr8(aiOf() + 0x20, 4);
        if (e.h(0xF2540, 0, game, unit, 0, 0, T, 2, 0, 0, 1, 4)) return;
    }
    if (step() < 0x1E && d < 0xC) {
        aiw();
        if (e.h(0xF2C10, T, 0, 0, unit, 0, game, (uint32_t)(0xC - d) & 0xFF, 0)) return;
        walk();
    }
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t blk[8] = { 4, unit, T, 0, 0, 0, 0, 0 };
    uint32_t bl = 0x64;
    const uint32_t rec = e.h(0x1320, e.rd32(unit + 4));
    if (rec && (e.rd8(rec + 0x104) & e.rd32(bm + 0x10))) bl = 0x65;
    blk[5] = bl << 8;
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
    e.blk(0x75E10, blk, 8);
}
}
