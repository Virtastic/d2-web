// 1.13c D2Game+0x15E00 (AITHINK_Fn129_GenericSpawner): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_d040.h). tp: +0 control, +8 target, +0x14 target distance, +0x1C
// the row whose per-difficulty words (index = game byte +0x6D) are +0x56 (frame gap) and +0x62 (spawn cap). Control: +0x14 last
// spawn frame, +0x18 spawn count, +0x3C minion class.
// Rule: no target: idle 20 (0xF22C0, EAX = game, ESI = unit, stack 0x14). Class -1: 0x15730 (ECX = unit, stack = game, &ctl+0x3C)
// must succeed, else flag 0x20000 on the unit, 0xA496(unit, 0), and the 8-word command {0, unit, 0, 0, 0, 0x6400, 0, 0} to 0x75E10
// (stack = game, &block, 1). Distance > 20: idle. Count >= cap: flag, 0xF21E0 (EAX = 0, EDI = unit, stack = game, 0, 0). Frame gap
// (abs) below the row word: idle. Else last frame := game frame and 0xF3D20 (EAX = 0xA478(unit) room, ECX = 0x1130, EDX = class
// 0x210 if monstats count (dt+0xA80) > 0x210 else -1, stack = 0x1100, 0) spawns; success: count++, 0xF2120 (EDI = 0xA7, stack =
// game, target, 0, 0), else idle.
#pragma once
#include <cstdint>

namespace g15e00 {
template <class E> void GenericSpawner(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x15E00
    const uint32_t ctl = e.rd32(tp), row = e.rd32(tp + 0x1C), tgt = e.rd32(tp + 8);
    const int32_t dist = (int32_t)e.rd32(tp + 0x14);
    auto idle = [&]() { e.h(0xF22C0, game, 0, 0, 0, unit, 0x14); };
    auto flag = [&]() { if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x20000u); };
    if (!tgt) { idle(); return; }
    if (e.rd32(ctl + 0x3C) == 0xFFFFFFFFu) {
        if (!(e.h(0x15730, 0, unit, 0, 0, 0, game, ctl + 0x3C) & 0xFF)) {
            flag();
            e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
            uint32_t blk[8] = { 0, unit, 0, 0, 0, 0x6400, 0, 0 };
            e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
            e.blk(0x75E10, blk, 8);
            return;
        }
    }
    if (dist > 0x14) { idle(); return; }
    const uint32_t diff = e.rd8(game + 0x6D);
    if ((int32_t)e.rd32(ctl + 0x18) >= e.rd16s(row + diff * 2 + 0x62)) {
        flag();
        e.h(0xF21E0, 0, 0, 0, 0, 0, game, 0, 0);
        e.j(0xF21E0, unit);
        return;
    }
    const uint32_t frame = e.rd32(game + 0xA8);
    int32_t gap = (int32_t)(frame - e.rd32(ctl + 0x14));
    if (gap < 0) gap = -gap;
    if (gap < e.rd16s(row + diff * 2 + 0x56)) { idle(); return; }
    e.wr32(ctl + 0x14, frame);
    const uint32_t cls = e.rd32(dt + 0xA80) > 0x210 ? 0x210u : 0xFFFFFFFFu;
    const uint32_t y = e.h(0x1130, 0, unit);
    const uint32_t x = e.h(0x1100, 0, unit);
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (e.h(0xF3D20, room, y, cls, 0, 0, x, 0)) {
        e.wr32(ctl + 0x18, e.rd32(ctl + 0x18) + 1);
        e.h(0xF2120, 0, 0, 0, 0, 0, game, tgt, 0, 0);
        e.j(0xF2120, 0xA7);
        return;
    }
    idle();
}
}
