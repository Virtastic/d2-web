// 1.13c D2Game+0x166D0 (D2GAME_AI_SpecialState16): ECX = pGame, EDX = pUnit, stack = pAiTickParam (ret 4; the result is residue).
// Written from the 1.13c disassembly as a rule over E (see d2game_15e00.h). tp: +0 control (ctl: +0x18 / +0x1C), +8 target unit, +0x14 distance.
// F = the owner: the unit's flag word +0xC8 bit 10 set, then 0xE03A0 (EAX = unit +0x94, ECX = game, EDX = unit +0x98), else 0. Rows: monstats 0x1EC at
// table +0x32EE0 and 0x1ED at +0x33088 (zero when the table count, dt +0xA80, is not above the index). Row words index by game byte +0x6D.
// 0xA68E(unit, 0x8F) == 0 or F == 0: stop = 0x9D230(game, unit, ad, 0) with ad = the unit's ai +0x28, then idle 1 (0xF22C0, EAX = game, ESI = unit).
// Else far (distance > 0x18, or 0xDFA90(F) or 0xA514(F) non-zero) or ctl +0x18 == 0: 0xF2420(rowA +0x5C word, rowA +0x170 word, rowA +0x180 byte) and
// ad +0x14 := -1. Else near: distance >= rowB +0x5C word, or the 0x1180 roll(100) >= rowB +0x62 word: roll(100) < 50 sends 0xF2220 (EAX = 8, stack = game,
// target) else idles 0x14. Otherwise it casts: 0x1100 / 0x1130 of the target, two rolls of 2 * rowB +0x68 word when ctl +0x1C != 0, skill = rowA word
// +0x170 + 2 * ctl +0x18; negative idles 0x14, else 0xF2380 (EAX = skill with the low byte rowA +0x180 + ctl +0x18, ECX = unit, EBX = game, stack =
// skill, target, 0, 0).
#pragma once
#include <cstdint>

namespace g166d0 {
template <class E> void SpecialState16(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0x166D0
    const uint32_t ctl = e.rd32(tp), tgt = e.rd32(tp + 8);
    const int32_t dist = (int32_t)e.rd32(tp + 0x14);
    uint32_t F = 0;
    if (unit && ((e.rd32(unit + 0xC8) >> 10) & 1)) F = e.h(0xE03A0, e.rd32(unit + 0x94), game, e.rd32(unit + 0x98));
    const uint32_t chk = e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0x8F);
    const uint32_t seed = unit + 0x20, d = e.rd8(game + 0x6D) * 2;
    if (!chk || !F) {
        uint32_t ad = 0;
        if (unit && e.rd32(unit) == 1) { const uint32_t ai = e.rd32(unit + 0x14); if (ai) ad = e.rd32(ai + 0x28); }
        e.h(0x9D230, 0, game, unit, 0, 0, ad, 0);
        e.h(0xF22C0, game, 0, 0, 0, unit, 1);
        return;
    }
    const int32_t cnt = (int32_t)e.rd32(dt + 0xA80);
    const uint32_t tbl = e.rd32(dt + 0xA78);
    const uint32_t A = cnt > 0x1EC ? tbl + 0x32EE0 : 0, B = cnt > 0x1ED ? tbl + 0x33088 : 0;
    bool far = dist > 0x18;
    if (!far) { if (e.h(0xDFA90, F)) far = true; else if (e.h(0xA514, 0, 0, 0, 0, 0, F)) far = true; }
    if (far || !e.rd32(ctl + 0x18)) {
        e.h(0xF2420, 0, 0, 0, 0, 0, (uint32_t)e.rd16s(A + d + 0x5C), (uint32_t)e.rd16s(A + 0x170), (uint32_t)(int32_t)(int8_t)e.rd8(A + 0x180));
        if (unit && e.rd32(unit) == 1) { const uint32_t ai = e.rd32(unit + 0x14); if (ai) e.wr32(e.rd32(ai + 0x28) + 0x14, 0xFFFFFFFFu); }
        return;
    }
    bool send = false;
    if (dist >= e.rd16s(B + d + 0x5C)) send = true;
    else if ((int32_t)e.h(0x1180, 0x64, unit + 0x20) >= e.rd16s(B + d + 0x62)) send = true;
    if (send) {
        if ((int32_t)e.h(0x1180, 0x64, seed) < 0x32) e.h(0xF2220, 8, 0, 0, 0, 0, game, tgt);
        else e.h(0xF22C0, game, 0, 0, 0, unit, 0x14);
        return;
    }
    const uint32_t c18 = e.rd32(ctl + 0x18), c1c = e.rd32(ctl + 0x1C);
    e.h(0x1100, 0, tgt); e.h(0x1130, 0, tgt);
    if (c1c) {
        e.h(0x1180, (uint32_t)e.rd16s(B + d + 0x68) << 1, seed);
        e.h(0x1180, (uint32_t)e.rd16s(B + d + 0x68) << 1, seed);
    }
    const int32_t sk = e.rd16s(A + c18 * 2 + 0x170);
    if (sk < 0) { e.h(0xF22C0, game, 0, 0, 0, unit, 0x14); return; }
    e.h(0xF2380, ((uint32_t)sk & ~0xFFu) | e.rd8(A + c18 + 0x180), unit, 0, game, 0, (uint32_t)sk, tgt, 0, 0);
}
}
