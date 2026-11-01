// 1.13c D2Game+0x48FC0 (apply a skill-use hit packet: mark the target, update the monster's link, ask the client, re-send), transcribed from
// the 1.13c disassembly. Convention: stack = (game, player, target, word, &packet, flag), ret 0x18; EAX is not an input, the probe carries it
// in because the early exits leave it. Result (EAX at the exit, the function is void): EAX-in when player or target is null; the packet pointer
// when the packet is null (then 0) or the player's type is not 0; 0 when 0xABE0 or 0xA3BE fails; else the target's data pointer when its
// type is 1, or its type. Flow: target+0xC4 |= 0x4020200; 0xE0460 (EAX = target, stack 0; BL = 2 is a partial register); flag zero: 0xEFE40
// (ECX = game, EBX = player+0xD0, stack 0); monster = 0x96A40 (EAX = player, EBX = 1, ESI = 7, EDI = game); when non-null: client =
// [0xA4C0(player) + 0x9C] (player type 0), 0x8A690 (EAX = 0xFFFF, ECX = 0, stack client), a type-1 monster's [data+0x28] struct gets
// +0x2C = -1, +0x30 = 1, +0x28 = game, 0x97B50 (ECX = game, EDX = player, stack monster id, 0), 0xA478 (stack monster, 1, id) into 0xA44E, 0xE20A0
// (ECX = game, EDX = monster). Then 0xABF8 (ECX = [game+0x70], EDX = 0, stack packet word) and 0xABE0 (ECX = [game+0x70], EDX = player, stack
// packet dword +4, that result, byte [game+0x6D], &out; zero returns) fill out[]; 0x97E20 (EAX = player, stack game, target, 7, {packet dword
// +4, packet word, out[0]}; ret 0x10); target+0xC4 |= 0x80000000; a type-1 target's [data+0x28] struct gets +0x2C = player id, +0x30 = player type,
// +0x28 = game; when the word argument's low half is 0xFFFF: 0xA3BE (stack [game+0x70], out[0] low half, out[1]; zero returns), 0xA472 (stack target,
// 0xD, out[6], 0), 0x48BA0 (stack game, player, target, out[1]; ret 0x10), 0x48950 (EAX = player, ECX = game). Then 0x232B0 (stack target, 0;
// BL = 0x13) and a type-1 target with data clears the data bytes +4, +9, +0xA, +0xB, +5.
#pragma once
#include <cstdint>

namespace r48fc0 {
template <class E> uint32_t Do(E& e, uint32_t eaxIn, uint32_t game, uint32_t plr, uint32_t tgt, uint32_t w3, uint32_t pk, uint32_t flag)
{
    if (!plr || !tgt) return eaxIn;
    if (!pk) return 0;
    if (e.rd32(plr) != 0) return pk;
    e.wr32(tgt + 0xC4, e.rd32(tgt + 0xC4) | 0x4020200u);
    e.h(0xE0460, tgt, 0, 0, 0, 0, 0);
    if (!flag) e.h(0xEFE40, 0, game, 0, e.rd32(plr + 0xD0), 0, 0);
    const uint32_t mon = e.h(0x96A40, plr, 0, 0, 1, 7);
    if (mon) {
        const uint32_t client = e.rd32(plr) == 0 ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, plr) + 0x9C) : 0;
        e.h(0x8A690, 0xFFFFu, 0, 0, 0, 0, client);
        if (e.rd32(mon) == 1) {
            const uint32_t p = e.rd32(mon + 0x14), q = p ? e.rd32(p + 0x28) : 0;
            e.wr32(q + 0x2C, 0xFFFFFFFFu); e.wr32(q + 0x30, 1); e.wr32(q + 0x28, game);
        }
        const uint32_t id = e.rd32(mon + 0xC);
        e.h(0x97B50, 0, game, plr, 0, 0, id, 0);
        const uint32_t r = e.h(0xA478, 0, 0, 0, 0, 0, mon, 1, id);
        e.h(0xA44E, 0, 0, 0, 0, 0, r);
        e.h(0xE20A0, 0, game, mon);
    }
    const uint32_t loc1 = e.rd32(pk + 4), loc2 = e.rd32(pk) & 0xFFFFu, g70 = e.rd32(game + 0x70);
    const uint32_t ra = e.h(0xABF8, 0, g70, 0, 0, 0, loc2);
    volatile uint32_t out[16] = {};
    if (!e.h(0xABE0, 0, g70, plr, 0, 0, loc1, ra, e.rd8(game + 0x6D), e.stk((const void*)out))) return 0;
    const uint32_t arr[3] = { loc1, loc2, out[0] };
    e.h(0x97E20, plr, 0, 0, 0, 0, game, tgt, 7, e.stk(arr));
    e.blk(0x97E20, arr, 3);
    e.wr32(tgt + 0xC4, e.rd32(tgt + 0xC4) | 0x80000000u);
    if (e.rd32(tgt) == 1) {
        const uint32_t p = e.rd32(tgt + 0x14), q = p ? e.rd32(p + 0x28) : 0;
        e.wr32(q + 0x2C, e.rd32(plr + 0xC)); e.wr32(q + 0x30, e.rd32(plr)); e.wr32(q + 0x28, game);
    }
    if ((w3 & 0xFFFFu) == 0xFFFFu) {
        const uint32_t o1 = out[1];
        if (!e.h(0xA3BE, 0, 0, 0, 0, 0, g70, out[0] & 0xFFFFu, o1)) return 0;
        e.h(0xA472, 0, 0, 0, 0, 0, tgt, 0xD, out[6], 0);
        e.h(0x48BA0, 0, 0, 0, 0, 0, game, plr, tgt, o1);
        e.h(0x48950, plr, game);
    }
    e.h(0x232B0, 0, 0, 0, 0, 0, tgt, 0);
    const uint32_t tt = e.rd32(tgt);
    if (tt != 1) return tt;
    const uint32_t p = e.rd32(tgt + 0x14);
    if (p) { e.wr8(p + 4, 0); e.wr8(p + 9, 0); e.wr8(p + 0xA, 0); e.wr8(p + 0xB, 0); e.wr8(p + 5, 0); }
    return p;
}
}
