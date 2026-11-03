// 1.13c D2Game+0x16270 (AITHINK_Fn093_ArcaneTower): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_15e00.h). tp: +0 control (ctl), +8 target, +0x1C monstats row. ctl: +0x14 phase,
// +0x18 shots left, +0x1C next frame. Row words (index = game byte +0x6D): +0x56 shots, +0x5C/+0x62 phase 0 gaps, +0x68 phase 1 shots, +0x6E/+0x74 gaps,
// +0x170 skill, +0x180 skill byte.
// Rule: shots left 0 loads row +0x56. Frame below next frame: idle 10 (0xF22C0, EAX = game, ESI = unit). Phase 0 with skill >= 0: skill 0xF2380 (EAX =
// frame hi16 | skill & 0xFF00 | row byte, ECX = unit, EBX = game, stack = skill, target, 0, 0), shots--, >0: next = row +0x5C + frame; else phase 1,
// shots = row +0x68, next = row +0x62 + frame. Else: 0xA496(unit, 0), the 8-word command {4, unit, target, 0, 0, flag << 8, 0, 0} to 0x75E10 (stack = game,
// &block, 1) with flag 0x65 if the unit's monstats row (0x1320, EAX = unit id) has byte +0x104 & ga[+0x10] (the D2Game global at 0xF8BE0) else 0x64;
// shots--, >0: next = row +0x74 + frame; else phase 0, shots = row +0x56, next = row +0x6E + frame.
#pragma once
#include <cstdint>

namespace g16270 {
template <class E> void ArcaneTower(E& e, uint32_t game, uint32_t unit, uint32_t tp, uint32_t ga)
{
    // 1.13c D2Game+0x16270
    const uint32_t ctl = e.rd32(tp), row = e.rd32(tp + 0x1C), tgt = e.rd32(tp + 8);
    const uint32_t d = e.rd8(game + 0x6D) * 2;
    if (!e.rd32(ctl + 0x18)) e.wr32(ctl + 0x18, (uint32_t)e.rd16s(row + 0x56 + d));
    const uint32_t frame = e.rd32(game + 0xA8);
    if ((int32_t)frame < (int32_t)e.rd32(ctl + 0x1C)) { e.h(0xF22C0, game, 0, 0, 0, unit, 0xA); return; }
    const int32_t skill = e.rd16s(row + 0x170);
    int32_t shots;
    if (skill >= 0 && !e.rd32(ctl + 0x14)) {
        e.h(0xF2380, (frame & 0xFFFF0000u) | ((uint32_t)skill & 0xFF00u) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)skill, tgt, 0, 0);
        shots = (int32_t)e.rd32(ctl + 0x18) - 1; e.wr32(ctl + 0x18, (uint32_t)shots);
        if (shots <= 0) {
            e.wr32(ctl + 0x14, 1);
            e.wr32(ctl + 0x18, (uint32_t)e.rd16s(row + 0x68 + d));
            e.wr32(ctl + 0x1C, (uint32_t)e.rd16s(row + 0x62 + d) + e.rd32(game + 0xA8));
            return;
        }
        e.wr32(ctl + 0x1C, (uint32_t)e.rd16s(row + 0x5C + d) + e.rd32(game + 0xA8));
        return;
    }
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t flag = 0x64;
    if (unit) {
        const uint32_t ms = e.h(0x1320, e.rd32(unit + 4));
        if (ms && (e.rd8(ms + 0x104) & e.rd32(ga + 0x10))) flag = 0x65;
    }
    uint32_t blk[8] = { 4, unit, tgt, 0, 0, flag << 8, 0, 0 };
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
    e.blk(0x75E10, blk, 8);
    shots = (int32_t)e.rd32(ctl + 0x18) - 1; e.wr32(ctl + 0x18, (uint32_t)shots);
    if (shots <= 0) {
        e.wr32(ctl + 0x14, 0);
        e.wr32(ctl + 0x18, (uint32_t)e.rd16s(row + 0x56 + d));
        e.wr32(ctl + 0x1C, (uint32_t)e.rd16s(row + 0x6E + d) + e.rd32(game + 0xA8));
        return;
    }
    e.wr32(ctl + 0x1C, (uint32_t)e.rd16s(row + 0x74 + d) + e.rd32(game + 0xA8));
}
}
