// 1.13c D2Game+0x17CB0 (AITHINK_Fn120_Overseer): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h). tp: +0 control (its +0x14 is the next-cast frame), +8 target, +0x14 range,
// +0x18 mode, +0x1C monstats row. wNN = row short +NN indexed by the game byte +0x6D; the seed roll is the unit's +0x20/+0x24 step mod 100.
// Prologue: 0x5A690 (EAX = game, ECX = target, EDX = unit); minion p = 0xE09D0(EAX = unit, stack = game): a player (type 0) in mode 0 or 0x11 or a
// monster (type 1) in mode 0 or 0xC is none, a monster needs 0xE0A60(EAX = p, ECX = unit, EDX = its mode, stack = game) != 0, any other type is none.
// Whip: skill +0x170 >= 0, a type-1 unit with data whose +0x54 is 3 or 0x13, game frame +0xA8 > control +0x14, p set and 0xB0840(EAX = unit,
// EDX = skill) != 0: 0xF2380 (EAX = that result with low byte +0x180, ECX = unit, EDX = skill, EBX = game, stack = skill, p, 0, 0), control +0x14 =
// frame + w56. Mode != 0 and roll < w74: 0xA496(unit, 0), the command {roll < w7A ? 5 : 4, unit, target, 0, 0, flag << 8, 0, 0} through 0x75E10
// (stack = game, &block, 1); flag 0x65 when the unit's monstats row (0x1320) byte +0x104 & ga[+0x14 for 5, +0x10 for 4] (global 0xF8BE0), else 0x64.
// Otherwise 0xB17B0 (EAX = 1, ECX = &{0x240, 0x190, 0x32, 0, 0, 0}, EDX = game, stack = unit, 0x6FC35C40) fills b3..b5 of the block:
// +0x172 >= 0, b3 and roll < w5C and 0xB0840(unit, +0x172) cast with byte +0x181 at b3; +0x174 != 0, b4, roll < w62, 0xA514(unit) != 2 and
// 0xB0840(unit, +0x174) cast with byte +0x182 at b4. b5 set: range < w68 - w6E steps to it with 0xF2D80 (EAX = target, stack = game, unit,
// (byte w68 - range) & 0xFF, 0); range > w68 + w6E attacks; else 0x1180(EAX = 100, ECX = &seed) < 50 runs 0xF26B0 (EAX = 0, ECX = unit,
// stack = game, target, 5), else idle. b5 clear: roll < 60 attacks, else idle. Attack = 0xF2540 (ECX = game, EDX = unit, stack = target, 2, 0, 0,
// 1, 0); idle = 0xF22C0 (EAX = game, ESI = unit, stack = 10).
#pragma once
#include <cstdint>

namespace g17cb0 {
template <class E> uint32_t Roll(E& e, uint32_t unit)
{
    const uint64_t n = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5u + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)n); e.wr32(unit + 0x24, (uint32_t)(n >> 32));
    return (uint32_t)n % 100u;
}
template <class E> void Overseer(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x17CB0
    const uint32_t ctl = e.rd32(tp), tgt = e.rd32(tp + 8), range = e.rd32(tp + 0x14), mode = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C);
    auto w = [&](uint32_t off) { return e.rd16s(row + off + e.rd8(game + 0x6D) * 2); };
    e.h(0x5A690, game, tgt, unit);
    uint32_t p = e.h(0xE09D0, unit, 0, 0, 0, 0, game);
    if (p) {
        const uint32_t t = e.rd32(p), m = e.rd32(p + 0x10);
        if (t == 0) { if (m == 0 || m == 0x11) p = 0; }
        else if (t == 1) { if (m == 0 || m == 0xC) p = 0; else if (!e.h(0xE0A60, p, unit, 0, 0, 0, game)) p = 0; }
        else p = 0;
    }
    if (e.rd16s(row + 0x170) >= 0 && unit && e.rd32(unit) == 1 && e.rd32(unit + 0x14)) {
        const uint32_t k = e.rd32(e.rd32(unit + 0x14) + 0x54);
        if ((k == 3 || k == 0x13) && (int32_t)e.rd32(game + 0xA8) > (int32_t)e.rd32(ctl + 0x14) && p) {
            const uint32_t sk = (uint32_t)e.rd16s(row + 0x170);
            const uint32_t ok = e.h(0xB0840, unit, 0, sk);
            if (ok) {
                e.h(0xF2380, (ok & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, sk, p, 0, 0);
                e.wr32(ctl + 0x14, (uint32_t)(w(0x56) + (int32_t)e.rd32(game + 0xA8)));
                return;
            }
        }
    }
    if (mode && (int32_t)Roll(e, unit) < w(0x74)) {
        const uint32_t cmd = (int32_t)Roll(e, unit) < w(0x7A) ? 5 : 4;
        e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
        uint32_t flag = 0x64;
        if (unit) {
            const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
            if (ms && (e.rd8(ms + 0x104) & e.rd32(ga + (cmd == 5 ? 0x14 : 0x10)))) flag = 0x65;
        }
        uint32_t blk[8] = { cmd, unit, tgt, 0, 0, flag << 8, 0, 0 };
        e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
        e.blk(0x75E10, blk, 8);
        return;
    }
    uint32_t blk[6] = { 0x240, 0x190, 0x32, 0, 0, 0 };
    const uint32_t in[6] = { 0x240, 0x190, 0x32, 0, 0, 0 };
    e.h(0xB17B0, 1, e.stk(blk), game, 0, 0, unit, 0x6FC35C40u);
    e.blk(0xB17B0, in, 6, 0x3F);   // retail logs the block at hook entry, before the hook's out-words land
    const uint32_t b3 = blk[3], b4 = blk[4], b5 = blk[5];
    if (e.rd16s(row + 0x172) >= 0 && b3 && (int32_t)Roll(e, unit) < w(0x5C)) {
        const uint32_t sk = (uint32_t)e.rd16s(row + 0x172);
        const uint32_t ok = e.h(0xB0840, unit, 0, sk);
        if (ok) { e.h(0xF2380, (ok & ~0xFFu) | e.rd8(row + 0x181), unit, 0, game, 0, sk, b3, 0, 0); return; }
    }
    if (e.rd16s(row + 0x174) != 0 && b4 && (int32_t)Roll(e, unit) < w(0x62) && e.h(0xA514, 0, 0, 0, 0, 0, unit) != 2) {
        const uint32_t sk = (uint32_t)e.rd16s(row + 0x174);
        const uint32_t ok = e.h(0xB0840, unit, 0, sk);
        if (ok) { e.h(0xF2380, (ok & ~0xFFu) | e.rd8(row + 0x182), unit, 0, game, 0, sk, b4, 0, 0); return; }
    }
    bool attack;
    if (b5) {
        const int32_t w68 = w(0x68), w6e = w(0x6E);
        if ((int32_t)range < w68 - w6e) {
            const uint32_t by = (e.rd8(row + 0x68 + e.rd8(game + 0x6D) * 2) - range) & 0xFF;
            e.h(0xF2D80, tgt, 0, 0, 0, 0, game, unit, by, 0);
            return;
        }
        if ((int32_t)range > w6e + w68) attack = true;
        else if ((int32_t)e.h(0x1180, 100, unit + 0x20) < 50) { e.h(0xF26B0, 0, unit, 0, 0, 0, game, tgt, 5); return; }
        else attack = false;
    } else attack = (int32_t)Roll(e, unit) < 60;
    if (attack) e.h(0xF2540, 0, game, unit, 0, 0, tgt, 2, 0, 0);
    else e.h(0xF22C0, game, 0, 0, 0, unit, 10);
}
}
