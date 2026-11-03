// 1.13c D2Game+0x173D0 (AI_WanderStep): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_15e00.h). tp+0 = ctl: +0x14/+0x18 offsets, +0x1C step counter. Unit: +0 type, +0x14 ai,
// ai+0x28 = ad (+0x20 pending record P, +0x28/+0x2C/+0x30 the target key), ai+0x54 mode, +0x20 the seed (a pointer to the callees).
// Rule: P with word +8 == 8, or a unit with ai mode 3/0x13, or a non-zero result of 0x21490(unit, &local), runs the stop sequence (0x212F0 command {?, ?, 8}
// when not pending, 0x216A0, 0x9D230(game, unit, ctl, 0), idle 10). Else the target key resolves (0xE03A0, EAX/ECX/EDX = key words +0x30/+0x28/+0x2C): a
// different unit moves to P+0xC/+0x10 plus ctl +0x14/+0x18 (0x20EC0, 0xF2540) or idles 0xF without P; the unit itself wanders: roll(100) < 30 idles 10, else
// step = ctl+0x1C + 1 (100 wraps to 0), x/y from 0x1100/0x1130, R = roll(10) and one sign roll (0x1450), the step bands <0x19, <0x32, <0x4B, rest pick the
// offsets, 0xF2540(ECX game, EDX unit, stack 0, 2, x, y, 1, 4): success sends the 0x212F0 command {?, ?, 2, x, y} and keeps step, failure stores step + 0x19.
#pragma once
#include <cstdint>

namespace g173d0 {
template <class E> void WanderStep(E& e, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x173D0
    const uint32_t ctl = e.rd32(tp);
    uint32_t P = 0;
    const bool u1 = unit && e.rd32(unit) == 1;
    uint32_t ai = u1 ? e.rd32(unit + 0x14) : 0;
    uint32_t ad = ai ? e.rd32(ai + 0x28) : 0;
    if (ad) {
        P = e.rd32(ad + 0x20);
        if (P && e.rd32(P + 8) == 8) {
            e.h(0x216A0, unit);
            e.h(0x9D230, 0, game, unit, 0, 0, ctl, 0);
            e.h(0xF22C0, game, 0, 0, 0, unit, 0xA);
            return;
        }
    }
    uint32_t loc = 0;
    e.h(0x21490, unit, 0, 0, 0, 0, e.stk(&loc), 0);
    const bool dying = u1 && ai && (e.rd32(ai + 0x54) == 3 || e.rd32(ai + 0x54) == 0x13);
    if (dying || loc) {
        uint32_t b[5] = { 0, 0, 8, 0, 0 };
        e.h(0x212F0, unit, 0, 0, 0, 0, game, e.stk(b));
        e.blk(0x212F0, b, 5, 0x4);
        e.h(0x216A0, unit);
        e.h(0x9D230, 0, game, unit, 0, 0, ctl, 0);
        e.h(0xF22C0, game, 0, 0, 0, unit, 0xA);
        return;
    }
    uint32_t tgt = 0;
    if (u1) {
        ai = e.rd32(unit + 0x14);
        ad = ai ? e.rd32(ai + 0x28) : 0;
        const uint32_t k = ad ? e.rd32(ad + 0x28) : 0;
        if (k) tgt = e.h(0xE03A0, e.rd32(ad + 0x30), k, e.rd32(ad + 0x2C));
    }
    if (tgt != unit) {
        if (!P) { e.h(0xF22C0, game, 0, 0, 0, unit, 0xF); return; }
        const uint32_t x = e.rd32(P + 0xC) + e.rd32(ctl + 0x14), y = e.rd32(P + 0x10) + e.rd32(ctl + 0x18);
        e.h(0x20EC0, unit, 0, 0, 0, 0, game);
        e.h(0xF2540, 0, game, unit, 0, 0, 0, 2, x, y, 1, 0);
        return;
    }
    if ((int32_t)e.h(0x1180, 0x64, unit + 0x20) < 0x1E) { e.h(0xF22C0, game, 0, 0, 0, unit, 0xA); return; }
    int32_t step = (int32_t)e.rd32(ctl + 0x1C) + 1;
    if (step >= 0x64) step = 0;
    int32_t x = (int32_t)e.h(0x1100, 0, unit), y = (int32_t)e.h(0x1130, 0, unit);
    int32_t R = (int32_t)e.h(0x1180, 0xA, unit + 0x20);
    if (e.h(0x1450, 0, 0, 0, 0, unit + 0x20, 2)) R = -R;
    if (step < 0x19) { x += R; y -= 5; }
    else if (step < 0x32) { x += 5; y += R; }
    else if (step < 0x4B) { x += R; y += 5; }
    else { x -= 5; y += R; }
    if (e.h(0xF2540, 0, game, unit, 0, 0, 0, 2, (uint32_t)x, (uint32_t)y, 1, 4)) {
        uint32_t b[5] = { 0, 0, 2, (uint32_t)x, (uint32_t)y };
        e.h(0x212F0, unit, 0, 0, 0, 0, game, e.stk(b));
        e.blk(0x212F0, b, 5, 0x1C);
        e.wr32(ctl + 0x1C, (uint32_t)step);
        return;
    }
    e.wr32(ctl + 0x1C, (uint32_t)step + 0x19);
}
}
