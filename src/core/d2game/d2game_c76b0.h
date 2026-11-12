// 1.13c D2Game+0xC76B0: object operate callback "tainted sun altar" (table entry). ecx = the operate block.
// Without quest record 0x0A in the game's quest control (+0x10F4): 0. The player's quest flags are the word
// +0x10 + 4 * the game's difficulty byte +0x6D of the player data (0xA4C0). With quest 0x0B bit 1 or bit 0 set
// (0xA568: flags, quest, bit; the answer 1 counts), a player who has bit 0 of quest 0x0A, or holds the item
// "vip " or "hst " (0xE660: eax = player, ebx = game; stack: code), only hears the sound 0x13 (0xDFFD0: esi =
// player; stack 0x13, player). An object past its neutral mode: 0. d = the record's data +0x18.
//   record byte +9 clear: the object goes to mode 1 (0xA4A2), d byte +4 = 1, d +8 = 2, d +0x9C = 0, the
//     players are visited (0xDFCC0: eax 0, ebx = callback 0xC3630, edi = game; stack: player), which leaves
//     the wanted count in d +0x9C; the object's level is read (0xA478, 0xA556), its drop code +0xB8 becomes
//     "vip ", one item per count is made (0x11A70: ebx = game; stack: object, 7, the level word, 0, -1, 0) and
//     flagged (0xA65E: item, 0x10, 1), the chest drop runs (0x6B570: block, 4), the control's seed (+0x18)
//     steps once and step % 5 + 5 gold piles drop (0x6AFA0: eax "gld ", ebx = object; stack: game, 2; no
//     control is fatal line 0x1FB), and quest record 9 of the record's game, when there, has the made count
//     added to its data +0x24.
//   record byte +9 set: d +0x10 = the object's room (0xA478), d +0x98 = its level (0xA556), mode 1, d +8 = 2,
//     d byte +5 = 1, d +0xC = the object's id (-1 without one), d byte +4 = 1; 0xC5E90 (eax = the record's game
//     +4; stack: record), with the module debug flag 0xCBE70 (edx = the text; stack: that game, 4), the state
//     moves to 4 (0x35EB0: eax 4, esi = record) and is sent (0x35E90: eax = that game); d +0x9C = 0 and the
//     same player visit; without quest 0x0B bit 0, bits 1 and 0xD are set (0xA57A) and bit 0xE cleared
//     (0xA586); three more visits with the object on the stack (callbacks 0xC36B0, 0xC5ED0, 0xC5090) and
//     0x33920 (eax = game; stack: record +0xE0, 0xD); the level of d +0x10 is read, the items are made as
//     above, their count goes to 0xC2380 (eax = record; stack: count), then the chest drop and gold as above,
//     a visit of the record's game (callback 0xC5260; stack 0) and 0x33AF0 (ebx 0x0A; stack: record, callback
//     0xC5E30).
// The result is 0. dbg = the module debug flag (0x111C20).
#pragma once
#include <cstdint>
#include "d2game_objinit.h"
#include "d2game_objop.h"
#include "d2game_fatal.h"

namespace gc76b0 {
template <class E> void Visit(E& e, uint32_t cb, uint32_t game, uint32_t arg)
{
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | cb, 0, arg);
    e.j(0xDFCC0, game);
}
template <class E> uint32_t Make(E& e, uint32_t op, uint32_t d, uint32_t* lvl)
{
    const uint32_t lp = (uint32_t)(uintptr_t)lvl;
    uint32_t made = 0;
    for (int32_t i = 0; i < (int32_t)e.rd32(d + 0x9C); i++) {
        const uint32_t it = e.h(0x11A70, 0, 0, 0, e.rd32(op), 0, e.rd32(op + 4), 7, lp, 0, 0xFFFFFFFFu, 0);
        e.blk(0x11A70, lp, 1);
        if (it) { e.h(0xA65E, 0, 0, 0, 0, 0, it, 0x10, 1); made++; }
    }
    return made;
}
template <class E> bool Gold(E& e, uint32_t op, uint32_t mod)
{
    e.h(0x6B570, 0, 0, 0, 0, 0, op, 4);
    const uint32_t ctl = e.rd32(e.rd32(op) + 0x10F4);
    if (!ctl) { d2gfatal::Fatal(e, 0x1FB, mod); return false; }
    const uint32_t n = gobjop::Roll(e, ctl + 0x18) % 5 + 5;
    for (uint32_t i = 0; i < n; i++) e.h(0x6AFA0, 0x20646C67, 0, 0, e.rd32(op + 4), 0, e.rd32(op), 2);
    return true;
}
template <class E> uint32_t Operate(E& e, uint32_t op, uint32_t dbg, uint32_t mod = 0xB2000000u)
{
    const uint32_t game = e.rd32(op);
    const uint32_t q = gobjinit::Find(e, op, 0xA);
    if (!q) return 0;
    const uint32_t flags = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8)) + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, flags, 0xB, 1) == 1 || e.h(0xA568, 0, 0, 0, 0, 0, flags, 0xB, 0) == 1) {
        if (e.h(0xA568, 0, 0, 0, 0, 0, flags, 0xA, 0) == 1
            || e.h(0xE660, e.rd32(op + 8), 0, 0, e.rd32(op), 0, 0x20706976)
            || e.h(0xE660, e.rd32(op + 8), 0, 0, e.rd32(op), 0, 0x20747368)) {
            const uint32_t pl = e.rd32(op + 8);
            e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
            return 0;
        }
    }
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    uint32_t lvl = d;
    if (!e.rd8(q + 9)) {
        e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
        e.wr8(d + 4, 1);
        e.wr32(d + 8, 2);
        e.wr32(d + 0x9C, 0);
        Visit(e, 0xC3630, e.rd32(op), e.rd32(op + 8));
        lvl = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(op + 4)));
        e.wr32(e.rd32(op + 4) + 0xB8, 0x20706976);
        const uint32_t made = Make(e, op, d, &lvl);
        if (!Gold(e, op, mod)) return 0;
        uint32_t c = e.rd32(e.rd32(q + 4) + 0x10F4);
        if (!c) return 0;
        for (c = e.rd32(c); c; c = e.rd32(c + 0xF4))
            if (e.rd32(c) == 9) {
                const uint32_t d9 = e.rd32(c + 0x18);
                e.wr32(d9 + 0x24, e.rd32(d9 + 0x24) + made);
                break;
            }
        return 0;
    }
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, obj);
    e.wr32(d + 0x10, room);
    e.wr32(d + 0x98, e.h(0xA556, 0, 0, 0, 0, 0, room));
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1);
    e.wr32(d + 8, 2);
    e.wr8(d + 5, 1);
    e.wr32(d + 0xC, gobjinit::ObjId(e, e.rd32(op + 4)));
    e.wr8(d + 4, 1);
    if (e.rd8(q + 9)) {
        e.h(0xC5E90, e.rd32(q + 4), 0, 0, 0, 0, q);
        if (dbg) e.h(0xCBE70, 0, 0, 0xB20FB2E0u, 0, 0, e.rd32(q + 4), 4);
        e.h(0x35EB0, 4, 0, 0, 0, q);
        e.h(0x35E90, e.rd32(q + 4));
    }
    e.wr32(d + 0x9C, 0);
    Visit(e, 0xC3630, e.rd32(op), e.rd32(op + 8));
    if (e.rd8(q + 9)) {
        if (!e.h(0xA568, 0, 0, 0, 0, 0, flags, 0xB, 0)) {
            e.h(0xA57A, 0, 0, 0, 0, 0, flags, 0xB, 1);
            e.h(0xA57A, 0, 0, 0, 0, 0, flags, 0xB, 0xD);
            e.h(0xA586, 0, 0, 0, 0, 0, flags, 0xB, 0xE);
        }
        Visit(e, 0xC36B0, e.rd32(op), e.rd32(op + 4));
        Visit(e, 0xC5ED0, e.rd32(op), e.rd32(op + 4));
        Visit(e, 0xC5090, e.rd32(op), e.rd32(op + 4));
        e.h(0x33920, e.rd32(op), 0, 0, 0, 0, e.rd32(q + 0xE0), 0xD);
    }
    lvl = e.h(0xA556, 0, 0, 0, 0, 0, e.rd32(d + 0x10));
    e.wr32(e.rd32(op + 4) + 0xB8, 0x20706976);
    const uint32_t made = Make(e, op, d, &lvl);
    e.h(0xC2380, q, 0, 0, 0, 0, made);
    if (!Gold(e, op, mod)) return 0;
    Visit(e, 0xC5260, e.rd32(q + 4), 0);
    e.h(0x33AF0, 0, 0, 0, 0xA, 0, q, 0xB20C5E30u);
    return 0;
}
}
