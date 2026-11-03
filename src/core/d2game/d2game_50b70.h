// 1.13c D2Game+0x50B70 (SkillNec_ApplySkillToUnit: a necromancer curse / skill state put on a unit): ECX = unit, EDX = p (the request block); plain ret.
// Written from the 1.13c disassembly as a rule over E (HookEnv). p words: [0] game, [1] skill, [3] overlay flag, [4..6] state args,
// [7] state id, [8..12] stat ids, [13] state-id value, [14..18] stat values, [19] timer arg.
// Gate (each failure returns 0): unit set and a monster (type 1); 0xA514(unit) == 0; 0xE0A60(EAX unit, ECX p[1], stack p[0]) != 0;
// 0xDFA90(EAX unit) == 0; 0x9F120(EAX unit, stack 0xB) != 0. t := 0x4E230(EDI p[7], EBX p[13], ESI unit); t == 0 makes the state id -1.
// 0xA00B0 gets EAX = a 9-dword block {p[1], unit, p[4], p[5], p[6], id, t, p[19], 0x6FC6EB90 (the state-end callback)}; its null result returns 0.
// Five stats: id p[8+i] in [0, ItemStatCost count dt+0xBD4) and v := 0x4E230(EDI id, EBX p[14+i], ESI unit) nonzero go to 0xA6E2(list, id, v, 0).
// p[3] set: 0xA59E(unit, 0x6FD1C1DC, 0x3D7). 0xE0460(EAX unit, BL 1, stack 1). A game with the unit in mode 0xB (unit+0xD0) and of type 0 or 1 gets a
// 16-byte node from 0xA2E6 (ECX game+0x1C, EDX 0x10, stack 0x6FD1BE1C, 0xA1, 0): {unit, 0, old head, 0} pushed on game+0x111C (old head +0xC := node),
// then unit+0xD0 := 9. A monster with 0x15D0(EAX unit class, EDX 0x10) set and monster data (unit+0x14) gets data+0x38 := 3, data+0x34 := 0.
// 0x8EA80(ECX game, EDX unit, stack 0xA, game+0xA8 + p[6], 0, 0, 0); the result is 1.
#pragma once
#include <cstdint>

namespace g50b70 {
template <class E> uint32_t ApplySkillToUnit(E& e, uint32_t dt, uint32_t unit, uint32_t p)
{
    // 1.13c D2Game+0x50B70
    auto P = [&](uint32_t i) { return e.rd32(p + 4 * i); };
    const uint32_t game = P(0), skill = P(1);
    if (!unit || e.rd32(unit) != 1) return 0;
    if (e.h(0xA514, 0, 0, 0, 0, 0, unit)) return 0;
    if (!e.h(0xE0A60, unit, skill, 0, 0, 0, game)) return 0;
    if (e.h(0xDFA90, unit)) return 0;
    if (!e.h(0x9F120, unit, 0, 0, 0, 0, 0xB)) return 0;
    uint32_t id = P(7);
    const uint32_t t = e.h(0x4E230, 0, 0, 0, P(13), unit); e.edi(0x4E230, id);
    if (!t) id = 0xFFFFFFFFu;
    uint32_t blk[9] = { skill, unit, P(4), P(5), P(6), id, t, P(19), 0xB204EB90u };   // the callback 0x6FC6EB90 as the capture tags it
    const uint32_t list = e.h(0xA00B0); e.blk(0xA00B0, blk, 9);
    if (!list) return 0;
    for (uint32_t i = 0; i < 5; i++) {
        const int32_t s = (int32_t)P(8 + i);
        if (s < 0 || s >= (int32_t)e.rd32(dt + 0xBD4)) continue;
        const uint32_t v = e.h(0x4E230, 0, 0, 0, P(14 + i), unit); e.edi(0x4E230, (uint32_t)s);
        if (v) e.h(0xA6E2, 0, 0, 0, 0, 0, list, (uint32_t)s, v, 0);
    }
    if (P(3)) e.h(0xA59E, 0, 0, 0, 0, 0, unit, 0xB20FC1DCu, 0x3D7);
    e.h(0xE0460, unit, 0, 0, 0, 0, 1);
    if (game && e.rd32(unit + 0xD0) == 0xB && e.rd32(unit) <= 1) {
        const uint32_t old = e.rd32(game + 0x111C);
        const uint32_t n = e.h(0xA2E6, 0, e.rd32(game + 0x1C), 0x10, 0, 0, 0xB20FBE1Cu, 0xA1, 0);
        if (n) {
            e.wr32(n, unit); e.wr32(n + 4, 0); e.wr32(n + 8, 0); e.wr32(n + 12, 0);
            e.wr32(game + 0x111C, n);
            if (old) { e.wr32(n + 8, old); e.wr32(old + 12, n); }
            e.wr32(unit + 0xD0, 9);
        }
    }
    if (e.rd32(unit) == 1 && e.h(0x15D0, e.rd32(unit + 4), 0, 0x10)) {
        const uint32_t md = e.rd32(unit + 0x14);
        if (md) { e.wr32(md + 0x38, 3); e.wr32(md + 0x34, 0); }
    }
    e.h(0x8EA80, 0, game, unit, 0, 0, 0xA, e.rd32(game + 0xA8) + P(6), 0, 0);
    return 1;
}
}
