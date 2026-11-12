// 1.13c D2Game+0xCA290: object operate callback "scroll chest" (table entry). ecx = the operate block. The
// object is taken out of selection (0x36B90: eax = block); 0 from it: 0. The object's drop code (+0xB8)
// becomes "tr1 ". With the quest record 9: its data's counter +0x14 is filled by the player visit (callback
// 0xC3FC0) and one quality-7 item per count is made. Then the plain drop (0x6B570: block, 4) and the gold
// piles. The result is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"
#include "d2game_objchest.h"

namespace gca290 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    if (!e.h(0x36B90, op)) return 0;
    e.wr32(e.rd32(op + 4) + 0xB8, 0x20317274);
    if (const uint32_t q = gobjinit::Find(e, op, 9))
        gobjchest::Items(e, op, e.rd32(q + 0x18), 0x14, 0xC3FC0, 7, [](uint32_t) {});
    e.h(0x6B570, 0, 0, 0, 0, 0, op, 4);
    gobjchest::Gold(e, op);
    return 0;
}
}
