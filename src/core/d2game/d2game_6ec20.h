// 1.13c D2Game+0x6EC20: object operate callback "corpse" (table entry). ecx = the operate block. An object
// past its neutral mode (+0x10 non-zero): 1, nothing done. Otherwise the drop is made (0x6B570: block, 0), the
// object's room (0xA478) and its level (0xA556) are read, the class record fetched (0xA520: class), the
// object goes to mode 1 (0xA4A2) with its mode timer (0x8EA80), loses its operable bit, and the chest tail
// follows (collision, sound). The result is 1.
#pragma once
#include <cstdint>
#include "d2game_objop.h"

namespace g6ec20 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 1;
    e.h(0x6B570, 0, 0, 0, 0, 0, op, 0);
    const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(op + 4));
    e.h(0xA556, 0, 0, 0, 0, 0, room);
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(op + 0x10));
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), 1);
    gobjop::ModeTimer(e, op, rec);
    gobjop::ClearOperable(e, e.rd32(op + 4));
    gobjop::OpenTail(e, op, rec);
    return 1;
}
}
