// 1.13c D2Game+0x164A0 (AITHINK_Fn083_MosquitoNest): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_15e00.h). tp: +0 control (ctl), +8 target, +0x14 target distance, +0x1C the
// monstats row; row words (index = game byte +0x6D): +0x56 spawn cap, +0x5C activation distance, +0x62 spawn delay, +0x170 skill, +0x180 skill byte.
// ctl: +0x14 next spawn frame, +0x18 spawned count.
// Rule: distance over the activation distance, or count not over the cap and (skill < 0, frame <= next frame, or the spawn 0xF3D20 failing):
// idle 25 (0xF22C0, EAX = game, ESI = unit). Count over the cap: unit flag 0x20000, 0xA496(unit, 0), the 8-word command {0, unit, 0, 0, 0, 0x6400, 0, 0}
// to 0x75E10 (stack = game, &block, 1). Else spawn: 0xF3D20 (EAX = 0xA478(unit) room, ECX = 0x1130, EDX = class 0x14E if monstats count (dt+0xA80)
// > 0x14E else -1, stack = 0x1100, 0); success: count++, next frame = frame + delay, 0xF2380 (EAX = (target & ~0xFF) | row byte, ECX = unit,
// EBX = game, stack = skill, target, 0, 0).
#pragma once
#include <cstdint>

namespace g164a0 {
template <class E> void MosquitoNest(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x164A0
    const uint32_t ctl = e.rd32(tp), row = e.rd32(tp + 0x1C), tgt = e.rd32(tp + 8);
    const uint32_t diff = e.rd8(game + 0x6D);
    auto idle = [&]() { e.h(0xF22C0, game, 0, 0, 0, unit, 0x19); };
    if ((int32_t)e.rd32(tp + 0x14) > e.rd16s(row + diff * 2 + 0x5C)) { idle(); return; }
    if ((int32_t)e.rd32(ctl + 0x18) > e.rd16s(row + diff * 2 + 0x56)) {
        if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x20000u);
        e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
        uint32_t blk[8] = { 0, unit, 0, 0, 0, 0x6400, 0, 0 };
        e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
        e.blk(0x75E10, blk, 8);
        return;
    }
    if (e.rd16s(row + 0x170) < 0) { idle(); return; }
    const uint32_t frame = e.rd32(game + 0xA8);
    if ((int32_t)frame <= (int32_t)e.rd32(ctl + 0x14)) { idle(); return; }
    const uint32_t cls = e.rd32(dt + 0xA80) > 0x14E ? 0x14Eu : 0xFFFFFFFFu;
    const uint32_t y = e.h(0x1130, 0, unit);
    const uint32_t x = e.h(0x1100, 0, unit);
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!e.h(0xF3D20, room, y, cls, 0, 0, x, 0)) { idle(); return; }
    const uint32_t delay = (uint32_t)e.rd16s(row + diff * 2 + 0x62);
    e.wr32(ctl + 0x18, e.rd32(ctl + 0x18) + 1);
    e.wr32(ctl + 0x14, delay + frame);
    e.h(0xF2380, (tgt & ~0xFFu) | e.rd8(row + 0x180), unit, 0, game, 0, (uint32_t)e.rd16s(row + 0x170), tgt, 0, 0);
}
}
