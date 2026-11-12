// 1.13c D2Game+0xBC220: object init callback of the fourth act's seal quest (table entry). ecx = the init block
// ([0] game, [4] the object). No quest control (game +0x10F4): 0; no record 0x17 in its list, or the data's byte
// +0x11 set: the control. Otherwise byte +0x06 becomes 1 and word +0x08 the object's id (-1 without an object).
// The first clear byte of the five at +0x0C answers its index; all set but word +0x48 not 3 answers 5. Otherwise
// 0xBB3B0 runs (eax = data, ecx = game); with byte +0x01 set that answer is the result, else word +0x18 and byte
// +0x03 are cleared, bytes +0x02 and +0x01 become 1 and a quest timer starts (0x33AF0: record, the handler), whose
// answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace gbc220 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t ctl = e.rd32(e.rd32(op) + 0x10F4);
    if (!ctl) return 0;
    const uint32_t q = gobjinit::Find(e, op, 0x17);
    if (!q) return ctl;
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(d + 0x11)) return ctl;
    e.wr8(d + 6, 1);
    e.wr32(d + 8, gobjinit::ObjId(e, e.rd32(op + 4)));
    for (uint32_t i = 0; i < 5; i++)
        if (!e.rd8(d + 0xC + i)) return i;
    if (e.rd32(d + 0x48) != 3) return 5;
    const uint32_t r = e.h(0xBB3B0, d, e.rd32(op));
    if (e.rd8(d + 1)) return r;
    e.wr32(d + 0x18, 0);
    e.wr8(d + 3, 0);
    e.wr8(d + 2, 1);
    e.wr8(d + 1, 1);
    return e.h(0x33AF0, 0, 0, 0, 0, 0, q, 0xB2000000u | 0xBBDC0);
}
}
