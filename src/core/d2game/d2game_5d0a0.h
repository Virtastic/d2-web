// 1.13c D2Game+0x5D0A0: object operate callback "summit altar" (table entry). ecx = the operate block. The
// quest record 0x23 is looked up; none: 0. An object past its neutral mode (+0x10 non-zero): 0.
//   record flag +9 clear: with data +0x0C above 0 the players are visited (0xDFCC0: eax 0, ebx = callback
//     0x5CFF0, edi = game; stack 0); the scroll message 0x4E22 is sent (0x34B80: eax = player, ecx = object).
//   record flag set and state +0x0C below 4: with data +0x0C above 0 the visit runs through 0x5D080 (edi =
//     game); the scroll message is sent; a state below 2 is moved to 2 (0x35EB0: eax 2, esi = record); a
//     record byte +0x0B other than 3 becomes 3 and the players of the record's game (+4) are visited with
//     callback 0x58F90.
// Last, with the object still in mode 0, it goes to mode 1 (0xA4A2) and data +0x4C becomes 2. The result is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"

namespace g5d0a0 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t game = e.rd32(op);
    const uint32_t q = gobjinit::Find(e, op, 0x23);
    if (!q) return 0;
    const uint32_t data = e.rd32(q + 0x18);
    uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 0;
    if (!e.rd8(q + 9)) {
        if ((int32_t)e.rd32(data + 0xC) > 0) { e.h(0xDFCC0, 0, 0, 0, 0xB205CFF0u, 0, 0); e.j(0xDFCC0, game); }
        e.h(0x34B80, e.rd32(op + 8), e.rd32(op + 4), 0, 0, 0, 0x4E22);
    } else if (e.rd8(q + 0xC) < 4) {
        if ((int32_t)e.rd32(data + 0xC) > 0) { e.h(0x5D080); e.j(0x5D080, game); }
        e.h(0x34B80, e.rd32(op + 8), e.rd32(op + 4), 0, 0, 0, 0x4E22);
        if (e.rd8(q + 0xC) < 2) e.h(0x35EB0, 2, 0, 0, 0, q);
        if (e.rd8(q + 0xB) != 3) {
            e.wr8(q + 0xB, 3);
            e.h(0xDFCC0, 0, 0, 0, 0xB2058F90u, 0, 0);
            e.j(0xDFCC0, e.rd32(q + 4));
        }
    }
    obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 0;
    e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
    e.wr32(data + 0x4C, 2);
    return 0;
}
}
