// Shared steps of the 1.13c D2Game quest-chest operate callbacks. op = the operate block ([0] game, [4] object,
// [8] player).
//   Gold:  the quest control (game +0x10F4, fatal line 0x1FB without one) carries a seed pair at +0x18; one
//          step % 5 + 5 gold piles are dropped (0x6AFA0: eax = the code "gld ", ebx = object; stack: game, 2).
//   Items: the quest data's counter word is cleared, the players around are visited (0xDFCC0: eax 0, ebx = the
//          callback, edi = game; stack: player), which leaves the wanted count in that word, and one item per
//          count is made at the object (0x11A70: ebx = game; stack: object, quality, a local, 0, -1, the flag).
//          got(item) runs for every item made.
#pragma once
#include <cstdint>
#include "d2game_objop.h"
#include "d2game_fatal.h"

namespace gobjchest {
template <class E> bool Gold(E& e, uint32_t op)
{
    const uint32_t ctl = e.rd32(e.rd32(op) + 0x10F4);
    if (!ctl) { d2gfatal::Fatal(e, 0x1FB, 0xB2000000u); return false; }
    const uint32_t n = gobjop::Roll(e, ctl + 0x18) % 5 + 5;
    for (uint32_t i = 0; i < n; i++) e.h(0x6AFA0, 0x20646C67, 0, 0, e.rd32(op + 4), 0, e.rd32(op), 2);
    return true;
}
template <class E, class F> void Items(E& e, uint32_t op, uint32_t data, uint32_t cnt, uint32_t cb, uint32_t quality, F&& got)
{
    e.wr32(data + cnt, 0);
    e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | cb, 0, e.rd32(op + 8));
    e.j(0xDFCC0, e.rd32(op));
    uint32_t local = op;   // the callee's out word; nothing reads it back
    for (int32_t i = 0; i < (int32_t)e.rd32(data + cnt); i++) {
        const uint32_t it = e.h(0x11A70, 0, 0, 0, e.rd32(op), 0, e.rd32(op + 4), quality, (uint32_t)(uintptr_t)&local, 0, 0xFFFFFFFFu, 1);
        if (it) got(it);
    }
}
}
