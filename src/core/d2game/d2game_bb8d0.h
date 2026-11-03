// 1.13c D2Game+0xBB8D0 (OBJECTS_OperateFunction49_HellForge): ECX = pOp {+0 game, +4 object, +8 player}; always returns 0.
// Written from the 1.13c disassembly as a rule over E. Quest node: the 0x18-typed entry of the list at game +0x10F4 (first entry [list], next at +0xF4); +0x18 data, +0x14 / +0xB bytes, +4 list word,
// +9 / +0xC flags, +0xE0 filter. Data: +0 word hellforge mode, +3 / +4 bytes, +8 / +0xC / +0x14 dwords. Callees (all hooked): 0xA4C0 (player data, 1 stack) holds the player's quest-state pointers at
// +0x10 + 4 * difficulty (game byte +0x6D); 0xA568 (state, 0x1B, 0 / 1) == 1 for reward granted / pending denies with 0xFFFFD0(0x13, player; ESI = player). The objects row comes from 0xA520(class or -1).
// Object mode 0 (also a NULL object): find the soulstone item (0xE660, EAX = player, EBX = game, stack 0x2073736d); none: deny, and if node +9 and not node +0xC then 0x35EB0 (EAX = 1, ESI = node).
// Found: set mode 1 (0xA4A2), animation event 0x8EA80 (ECX = game, EDX = object, stack 1, (row +0xDC >> 8) + game +0xA8, 0, 0, 0), data word := 2, node +0x14 := 0, node +0xB := 3, 0xDFCC0
// (EAX = 0, EBX = code 0xB9970, EDI = node +4, stack 0), delete the item 0x366A0 (EAX = code, ECX = game, ESI = player). Object mode 2: the forge-hammer item must be found (0xE660), the player
// inventory (+0x60) must exist (0xA54A), its class record (0xA53E(item +4)) code (+0x80) must match, else deny. Hit counter data +0xC: ++ and stop while <= 2; then mode 3, data word := 3,
// delete the item, player data (0x1020, EAX = player, stack game): 0xA57A(flags, 0x1B, 0xD / 1), 0xA574(flags, node +0xE0), data +4 := 1, +3 := 1, +8 := 4, +0x14 := 1, party id
// (0xDFA60, EAX = player; low word != 0xFFFF runs 0x9C160(code 0xBAF20; EBX = 0, ESI = game)), 0x35EB0 (EAX = 4, ESI = node), node +0x14 := 0, node +0xB := 0xD, 0xDFCC0 (EBX = 0xB9970, EDI = node +4),
// 0x8EA80 (stack 7, (row +0xE4 >> 8) + frame), 0xDFCC0 (EBX = 0xB9920, EDI = game, stack player), 0x35E90 (EAX = game).
#pragma once
#include <cstdint>

namespace gbb8d0 {
template <class E> void HellForge(E& e, uint32_t pop)
{
    // 1.13c D2Game+0xBB8D0
    const uint32_t game = e.rd32(pop), obj = e.rd32(pop + 4), plr = e.rd32(pop + 8);
    const uint32_t list = e.rd32(game + 0x10F4);
    if (!list) return;
    uint32_t node = e.rd32(list);
    if (!node) return;
    while (e.rd32(node) != 0x18) { node = e.rd32(node + 0xF4); if (!node) return; }
    auto deny = [&]() { e.h(0xDFFD0, 0, 0, 0, 0, plr, 0x13, plr); };
    const uint32_t pq = e.h(0xA4C0, 0, 0, 0, 0, 0, plr);
    const uint32_t qf = e.rd32(pq + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x1B, 0) == 1 || e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x1B, 1) == 1) { deny(); return; }
    const uint32_t dex = e.rd32(node + 0x18);
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, obj ? e.rd32(obj + 4) : 0xFFFFFFFFu);
    const uint32_t mode = obj ? e.rd32(obj + 0x10) : 0;
    auto word = [&](uint32_t a, uint32_t v) { e.wr8(a, v); e.wr8(a + 1, v >> 8); };
    if (mode == 0) {
        if (!e.h(0xE660, plr, 0, 0, game, 0, 0x2073736D)) {
            deny();
            if (e.rd8(node + 9) && !e.rd8(node + 0xC)) e.h(0x35EB0, 1, 0, 0, 0, node);
            return;
        }
        e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
        e.h(0x8EA80, 0, game, obj, 0, 0, 1, (e.rd32(rec + 0xDC) >> 8) + e.rd32(game + 0xA8), 0, 0, 0);
        word(dex, 2);
        e.wr8(node + 0x14, 0); e.wr8(node + 0xB, 3);
        e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xB9970, 0, 0);
        e.j(0xDFCC0, e.rd32(node + 4));
        e.h(0x366A0, 0x2073736D, game, 0, 0, plr);
        return;
    }
    if (mode != 2) return;
    if (!e.h(0xE660, plr, 0, 0, game, 0, 0x20686668)) { deny(); return; }
    const uint32_t inv = e.rd32(plr + 0x60);
    if (!inv) { deny(); return; }
    const uint32_t item = e.h(0xA54A, 0, 0, 0, 0, 0, inv);
    if (!item) { deny(); return; }
    const uint32_t ir = e.h(0xA53E, 0, 0, 0, 0, 0, e.rd32(item + 4));
    if (e.rd32(ir + 0x80) != 0x20686668) { deny(); return; }
    const int32_t cnt = (int32_t)e.rd32(dex + 0xC) + 1;
    e.wr32(dex + 0xC, (uint32_t)cnt);
    if (cnt <= 2) return;
    e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 3);
    word(dex, 3);
    e.h(0x366A0, 0x20686668, game, 0, 0, plr);
    const uint32_t fl = e.h(0x1020, plr, 0, 0, 0, 0, game);
    e.h(0xA57A, 0, 0, 0, 0, 0, fl, 0x1B, 0xD);
    e.h(0xA57A, 0, 0, 0, 0, 0, fl, 0x1B, 1);
    e.h(0xA574, 0, 0, 0, 0, 0, fl, e.rd32(node + 0xE0));
    e.wr8(dex + 4, 1); e.wr8(dex + 3, 1); e.wr32(dex + 8, 4); e.wr32(dex + 0x14, 1);
    if ((e.h(0xDFA60, plr) & 0xFFFF) != 0xFFFF) e.h(0x9C160, 0, 0, 0, 0, game, 0xB2000000u | 0xBAF20);
    e.h(0x35EB0, 4, 0, 0, 0, node);
    e.wr8(node + 0x14, 0); e.wr8(node + 0xB, 0xD);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xB9970, 0, 0);
    e.j(0xDFCC0, e.rd32(node + 4));
    e.h(0x8EA80, 0, game, obj, 0, 0, 7, (e.rd32(rec + 0xE4) >> 8) + e.rd32(game + 0xA8), 0, 0, 0);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xB9920, 0, plr);
    e.j(0xDFCC0, game);
    e.h(0x35E90, game);
}
}
