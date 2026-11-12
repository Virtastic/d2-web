// 1.13c D2Game+0x65B40: object operate callback "scroll tree" (table entry). ecx = the operate block. The
// quest record 4 must exist (fatal line 0x828). With its flag +9 clear the object goes to mode 1 (0xA4A2) and
// that is all. A state (+0x0C) of 6 or more, or an object past its neutral mode: 0. A player whose quest flags
// (player data 0xA4C0, word +0x10 + 4 * the game's difficulty byte +0x6D) have quest 4 bit 0 or bit 1 (0xA568
// answers 1): 0. A player already holding "bkd " or "bks " (0xE660: eax = player, ebx = game; stack: code)
// only hears the sound 0x13 (0xDFFD0: esi = player; stack 0x13, player). Otherwise the sound 0x2D is played
// (stack 0x2D, 0), the object's drop code (+0xB8) becomes "bks ", the quest start is logged (0x635B0: record),
// the state is moved to 4 (0x35EB0: eax 4, esi = record) and the scroll is made (0x11A70: ebx = game; stack:
// object, 2, a local, 0, -1, 0). With a scroll: the record's handler +0xC4 becomes 0x654A0, its byte +0x14 is
// cleared, byte +0x0B becomes 2, the players of the record's game (+4) are visited (0xDFCC0: eax 0, ebx =
// callback 0x62A80, edi = that game; stack: player), the quest data takes byte +0x4B = 1, the scroll's id at
// +0x38, word +0x58 = 1, byte +0x78 = 1 and a count at +0x7C, and the object goes to mode 1. Either way data
// byte +0x47 = 1 and data +0x30 = the object's id (-1 without one). The result is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"
#include "d2game_fatal.h"

namespace g65b40 {
template <class E> uint32_t Operate(E& e, uint32_t op, uint32_t mod = 0xB2000000u)
{
    const uint32_t game = e.rd32(op);
    const uint32_t q = gobjinit::Find(e, op, 4);
    if (!q) { d2gfatal::Fatal(e, 0x828, mod); return 0; }
    if (!e.rd8(q + 9)) { e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1); return 0; }
    if (e.rd8(q + 0xC) >= 6) return 0;
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 0;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(op + 8));
    const uint32_t flags = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, flags, 4, 0) == 1) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, flags, 4, 1) == 1) return 0;
    if (e.h(0xE660, e.rd32(op + 8), 0, 0, e.rd32(op), 0, 0x20646B62) || e.h(0xE660, e.rd32(op + 8), 0, 0, e.rd32(op), 0, 0x20736B62)) {
        const uint32_t pl = e.rd32(op + 8);
        e.h(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
        return 0;
    }
    const uint32_t d = e.rd32(q + 0x18);
    e.h(0xDFFD0, 0, 0, 0, 0, e.rd32(op + 8), 0x2D, 0);
    e.wr32(e.rd32(op + 4) + 0xB8, 0x20736B62);
    e.h(0x635B0, 0, 0, 0, 0, 0, q);
    e.h(0x35EB0, 4, 0, 0, 0, q);
    uint32_t local = op;   // the callee's out word; nothing reads it back
    const uint32_t it = e.h(0x11A70, 0, 0, 0, e.rd32(op), 0, e.rd32(op + 4), 2, (uint32_t)(uintptr_t)&local, 0, 0xFFFFFFFFu, 0);
    if (it) {
        e.wr32(q + 0xC4, 0xB20654A0u);
        e.wr8(q + 0x14, 0);
        e.wr8(q + 0xB, 2);
        e.h(0xDFCC0, 0, 0, 0, 0xB2062A80u, 0, e.rd32(op + 8));
        e.j(0xDFCC0, e.rd32(q + 4));
        const uint32_t n = e.rd32(d + 0x7C);
        e.wr8(d + 0x4B, 1);
        e.wr32(d + 0x38, e.rd32(it + 0xC));
        e.wr32(d + 0x58, 1);
        e.wr8(d + 0x78, 1);
        e.wr32(d + 0x7C, n + 1);
        e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1);
    }
    e.wr8(d + 0x47, 1);
    e.wr32(d + 0x30, gobjinit::ObjId(e, e.rd32(op + 4)));
    return 0;
}
}
