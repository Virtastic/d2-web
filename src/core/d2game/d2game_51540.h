// 1.13c D2Game+0x51540, transcribed from the 1.13c disassembly: the Iron Golem skill server do-function.
// Convention: ECX = game, EDX = unit, stack skill id, skill level (ret 8). dt is the data-tables block (sgptDataTables).
// 0x4EDD0 (ECX = game, EDX = unit; skill, level) zero returns 0. The unit gets +0xC4 |= 0x40; the skill row (0x23C each at dt+0xB98,
// count dt+0xBA0, signed) or null; skill id 0 returns 0. 0x9EE50 (ECX = 0, EDX = &out; unit, 0, skill, level) below zero returns 0.
// The 9-dword request {1, unit, that result, 0, out, x, y, row byte +0xBE (0 when not below dt+0xBF0), 0xAC9A (D2Common 10786: unit,
// row +0xC0, skill, level)} takes x / y from the unit (0x1100 / 0x1130, ECX = unit), then 0xE07A0 (EAX = unit, ECX = game) and the
// target from 0xA598 (D2Common 10392: unit +0x2C; the unit itself counts as none) replaces them with the target's. 0xA3610 (EAX =
// &request; game) gives the golem; none returns 0. With a target: 0xA478 (10331) its room, 0xA44E (10885: room, type, id), 0xA550
// (10408), 0xA63A (11016), 0xA868 (10700: target, &w, &byte), 0xA4A2 (11090: target, 4) and 0xD60D0 (EAX = target id, ECX = golem;
// game, w, 1, 0). Then 0x50070 (EAX = golem, EBX = level; game, skill), 0x8E2A0 (EAX = a player unit's 0xA4C0 (10920) record +0x9C,
// else 0; EDI = golem) and 0xEFE40 (ECX = game, EBX = unit +0xD0; 0). Returns 1.
#pragma once
#include <cstdint>

namespace r51540 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t skill, uint32_t level)
{
    if (!e.h(0x4EDD0, 0, game, unit, 0, 0, skill, level)) return 0;
    if (unit) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    const uint32_t rec = ((int32_t)skill >= 0 && (int32_t)skill < (int32_t)e.rd32(dt + 0xBA0)) ? e.rd32(dt + 0xB98) + skill * 0x23C : 0;
    if (!skill) return 0;
    uint32_t out[4] = { 0, 0, 0, 0 };
    const uint32_t r9 = e.h(0x9EE50, 0, 0, e.stk(out), 0, 0, unit, 0, skill, level);
    if ((int32_t)r9 < 0) return 0;
    uint32_t bt = e.rd8(rec + 0xBE);
    if ((int32_t)bt >= (int32_t)e.rd32(dt + 0xBF0)) bt = 0;
    uint32_t q[9] = { 1, unit, r9, 0, out[0], 0, 0, bt, 0 };
    q[5] = e.h(0x1100, 0, unit);
    q[6] = e.h(0x1130, 0, unit);
    q[8] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(rec + 0xC0), skill, level);
    e.h(0xE07A0, unit, game);
    uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit) tgt = 0;
    if (tgt) { q[5] = e.h(0x1100, 0, tgt); q[6] = e.h(0x1130, 0, tgt); }
    const uint32_t obj = e.h(0xA3610, e.stk(q), 0, 0, 0, 0, game);
    e.blk(0xA3610, q, 9);
    if (!obj) return 0;
    if (tgt) {
        const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, tgt);
        e.h(0xA44E, 0, 0, 0, 0, 0, room, e.rd32(tgt), e.rd32(tgt + 0xC));
        e.h(0xA550, 0, 0, 0, 0, 0, tgt);
        e.h(0xA63A, 0, 0, 0, 0, 0, tgt);
        uint32_t w[2] = { 0, 0 };
        uint8_t by[4] = { 0, 0, 0, 0 };
        e.h(0xA868, 0, 0, 0, 0, 0, tgt, e.stk(w), e.stk(by));
        e.h(0xA4A2, 0, 0, 0, 0, 0, tgt, 4);
        e.h(0xD60D0, e.rd32(tgt + 0xC), obj, 0, 0, 0, game, w[0], 1, 0);
    }
    e.h(0x50070, obj, 0, 0, level, 0, game, skill);
    const uint32_t v = e.rd32(unit) == 0 ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C) : 0;
    e.ed(obj);
    e.h(0x8E2A0, v);
    e.edi(0x8E2A0, obj);
    e.h(0xEFE40, 0, game, 0, e.rd32(unit + 0xD0), 0, 0);
    return 1;
}
}
