// 1.13c D2Game+0x6B340: object operate callback "book shelf" (table entry). ecx = the operate block. An
// object past its neutral mode (+0x10 non-zero): 1, nothing done. Otherwise the object goes to mode 2
// (0xA4A2), loses its operable bit, its position is read (0xA544: object, out pair) and two seed steps pick
// the item code written to the object's word +0xB8: first step % 20 above 12 picks a book ("ibk " when the
// second step is odd, else "tbk "), otherwise a scroll ("isc " odd, "tsc " even). The item is then dropped
// (0x11A70: ebx = game; stack: object, 2, the position pair, 0, -1, 0). The result is 1.
#pragma once
#include <cstdint>
#include "d2game_objop.h"

namespace g6b340 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 1;
    e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 2);
    gobjop::ClearOperable(e, e.rd32(op + 4));
    uint32_t pos[2] = { 0, 0 };
    const uint32_t pp = (uint32_t)(uintptr_t)pos;
    e.h(0xA544, 0, 0, 0, 0, 0, e.rd32(op + 4), pp);
    const uint32_t first = gobjop::Roll(e, e.rd32(op + 0xC)) % 20;
    const uint32_t odd = gobjop::Roll(e, e.rd32(op + 0xC)) & 1;
    uint32_t code;
    if (first > 12) code = odd ? 0x206B6269u : 0x206B6274u;
    else code = odd ? 0x20637369u : 0x20637374u;
    e.wr32(e.rd32(op + 4) + 0xB8, code);
    e.h(0x11A70, 0, 0, 0, e.rd32(op), 0, e.rd32(op + 4), 2, pp, 0, 0xFFFFFFFFu, 0);
    e.blk(0x11A70, pp, 2);
    return 1;
}
}
