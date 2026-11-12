// 1.13c D2Game+0x6EB40: object operate callback "jungle stash" (table entry). ecx = the operate block. An
// object past its neutral mode (+0x10 non-zero): 1, nothing done. Otherwise the object goes to mode 1
// (0xA4A2), loses its operable bit, and with its class record (0xA520: class) two timers are queued (0x8EA80:
// ecx = game, edx = object): event 1 at frame + (record +0xDC >> 8) + 1 and event 10 at frame + record +0x17C
// + 1. The chest tail follows (collision, sound); last the object's timer record (0xB168: ecx = object), when
// there is one, takes the player's id (+0xC, -1 without a player) at +4. The result is 1.
#pragma once
#include <cstdint>
#include "d2game_objop.h"

namespace g6eb40 {
template <class E> uint32_t Operate(E& e, uint32_t op)
{
    const uint32_t obj = e.rd32(op + 4);
    if (obj && e.rd32(obj + 0x10)) return 1;
    e.h(0xA4A2, 0, 0, 0, 0, 0, obj, 1);
    gobjop::ClearOperable(e, e.rd32(op + 4));
    const uint32_t rec = e.h(0xA520, 0, 0, 0, 0, 0, e.rd32(op + 0x10));
    gobjop::ModeTimer(e, op, rec);
    const uint32_t game = e.rd32(op);
    e.h(0x8EA80, 0, game, e.rd32(op + 4), 0, 0, 10, e.rd32(game + 0xA8) + e.rd32(rec + 0x17C) + 1, 0, 0, 0);
    gobjop::OpenTail(e, op, rec);
    const uint32_t pl = e.rd32(op + 8);
    const uint32_t id = pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu;
    const uint32_t t = e.h(0xB168, 0, e.rd32(op + 4));
    if (t) e.wr32(t + 4, id);
    return 1;
}
}
