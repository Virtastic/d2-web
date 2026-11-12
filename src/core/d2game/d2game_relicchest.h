// Shared shape of three 1.13c D2Game object operate callbacks (table entries 0xEA390, 0xEA4C0, 0xEA5F0): the
// chests of the three relic parts. ecx = the operate block. The object is taken out of selection (0x36B90:
// eax = block); 0 from it: 0. The gold piles are dropped, the object's drop code (+0xB8) is set, and with the
// quest record 0x10 its data's counter +0x0C is filled by the player visit and one quality-2 item per count is
// made; each one made counts in the part's data word and sets the part's data byte. Last the plain drop
// (0x6B570: block, 4). The result is 0.
#pragma once
#include <cstdint>
#include "d2game_objinit.h"
#include "d2game_objchest.h"

namespace grelicchest {
struct Part { uint32_t code, cb, count, flag; };
template <class E> uint32_t Operate(E& e, uint32_t op, const Part& p)
{
    if (!e.h(0x36B90, op)) return 0;
    if (!gobjchest::Gold(e, op)) return 0;
    e.wr32(e.rd32(op + 4) + 0xB8, p.code);
    if (const uint32_t q = gobjinit::Find(e, op, 0x10)) {
        const uint32_t d = e.rd32(q + 0x18);
        gobjchest::Items(e, op, d, 0x0C, p.cb, 2, [&](uint32_t) { e.wr32(d + p.count, e.rd32(d + p.count) + 1); e.wr8(d + p.flag, 1); });
    }
    e.h(0x6B570, 0, 0, 0, 0, 0, op, 4);
    return 0;
}
}
