// 1.13c D2Game+0x18480 (AI_RollActionByTable): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +8 target, +0x18 mode, +0x1C monstats row; wNN = row short +NN
// indexed by the game byte +0x6D; the seed roll is the unit's +0x20/+0x24 step mod 100.
// Prologue (type-1 unit with data d, d +0x28 = x, x +0x20 = y): y +8 == 1 and y +0x14 > game frame +0xA8 asks 0xE03A0 (EAX = y +0xC, ECX = game,
// EDX = y +0x10) for t; t set and 0xDFA90(EAX = t) == 0 sets flag, and a t other than the target becomes it: 0xAB86(unit, t), mode = 0xA754(unit, t, 0).
// Every other outcome under y runs 0x20EC0 (EAX = unit, stack = game). No y: nothing.
// Mode != 0: no flag and roll >= w56 idles 0xF22C0 (EAX = game, ESI = unit, stack = w5C); else 0xA496(unit, 0) and the command {roll < w5C ? 5 : 4,
// unit, target, 0, 0, f << 8, 0, 0} through 0x75E10 (stack = game, &block, 1), f 0x65 when 0x1320(EAX = unit +4) byte +0x104 & ga[+0x14 for 5,
// +0x10 for 4] (global 0xF8BE0), else 0x64. Mode 0: no flag and roll >= w62 idles with w68; else 0xF2540 (ECX = game, EDX = unit, stack = target,
// 2, 0, 0, 1, 0).
#pragma once
#include <cstdint>

namespace g18480 {
template <class E> uint32_t Roll(E& e, uint32_t unit)
{
    const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n % 100u;
}
template <class E> void RollActionByTable(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x18480
    const uint32_t row = e.rd32(tp + 0x1C);
    uint32_t tgt = e.rd32(tp + 8), mode = e.rd32(tp + 0x18), flag = 0;
    auto w = [&](uint32_t off) { return e.rd16s(row + off + e.rd8(game + 0x6D) * 2); };
    uint32_t y = 0;
    if (unit && e.rd32(unit) == 1 && e.rd32(unit + 0x14)) {
        const uint32_t x = e.rd32(e.rd32(unit + 0x14) + 0x28);
        if (x) y = e.rd32(x + 0x20);
    }
    if (y) {
        bool done = false;
        if (e.rd32(y + 8) == 1 && (int32_t)e.rd32(y + 0x14) > (int32_t)e.rd32(game + 0xA8)) {
            const uint32_t t = e.h(0xE03A0, e.rd32(y + 0xC), game, e.rd32(y + 0x10));
            if (t && !e.h(0xDFA90, t)) {
                done = true; flag = 1;
                if (tgt != t) {
                    tgt = t;
                    e.h(0xAB86, 0, 0, 0, 0, 0, unit, t);
                    mode = e.h(0xA754, 0, 0, 0, 0, 0, unit, t, 0);
                }
            }
        }
        if (!done) e.h(0x20EC0, unit, 0, 0, 0, 0, game);
    }
    if (mode) {
        if (!flag && (int32_t)Roll(e, unit) >= w(0x56)) { e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)w(0x5C)); return; }
        const uint32_t cmd = (int32_t)Roll(e, unit) < w(0x5C) ? 5 : 4;
        e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
        uint32_t f = 0x64;
        if (unit) {
            const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
            if (ms && (e.rd8(ms + 0x104) & e.rd32(ga + (cmd == 5 ? 0x14 : 0x10)))) f = 0x65;
        }
        uint32_t blk[8] = { cmd, unit, tgt, 0, 0, f << 8, 0, 0 };
        e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
        e.blk(0x75E10, blk, 8);
        return;
    }
    if (!flag && (int32_t)Roll(e, unit) >= w(0x62)) { e.h(0xF22C0, game, 0, 0, 0, unit, (uint32_t)w(0x68)); return; }
    e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0, 1, 0);
}
}
