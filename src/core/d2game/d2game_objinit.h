// Shared step of the 1.13c D2Game object init callbacks that belong to a quest: the walk of the game's quest
// records. op = the init block ([0] game, [4] the object). The list sits behind the control at game +0x10F4
// (its first word is the head); a record's id is its first word and its successor is at +0xF4. The answer is
// the record of that id, or 0 without a control, a head or a match. A record's flag byte is at +0x09, its game
// at +0x04 and its quest data at +0x18.
#pragma once
#include <cstdint>

namespace gobjinit {
template <class E> uint32_t Find(E& e, uint32_t op, uint32_t id)
{
    uint32_t q = e.rd32(e.rd32(op) + 0x10F4);
    if (!q) return 0;
    for (q = e.rd32(q); q; q = e.rd32(q + 0xF4))
        if (e.rd32(q) == id) return q;
    return 0;
}
// the object's id (+0x0C), or -1 without an object
template <class E> uint32_t ObjId(E& e, uint32_t obj) { return obj ? e.rd32(obj + 0xC) : 0xFFFFFFFFu; }
}
