// 1.13c D2Game+0x6ECD0: object operate callback "urn, basket, jar" (table entry). ecx = the operate block.
// An object past its neutral mode (+0x10 non-zero): 1, nothing done. Otherwise the object's room (0xA478) and
// its level (0xA556) are read, the object goes to mode 1 (0xA4A2), loses its operable bit, the class record
// is fetched (0xA520: class) and the mode timer queued (0x8EA80). One seed step % 100 at 20 or less makes the
// drop (0x6B570: block, 0). The chest tail follows (collision, sound). The result is 1.
#pragma once
#include <cstdint>
#include "d2game_objop.h"

namespace g6ecd0 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 1;
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, obj);
    e.h(0xA556, 0, 0, 0, 0, 0, room);
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1);
    gobjop::ClearOperable(e, e.rd32(op + 4));
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(op + 0x10));
    gobjop::ModeTimer(e, op, rec);
    if (gobjop::Roll(e, e.rd32(op + 0xC)) % 100 <= 20) e.h(0x6B570, 0, 0, 0, 0, 0, op, 0);
    gobjop::OpenTail(e, op, rec);
    return 1;
}
}
