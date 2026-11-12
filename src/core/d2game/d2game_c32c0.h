// 1.13c D2Game+0xC32C0: object init callback "harem blocker" (table entry). ecx = the init block ([0] game, [4]
// the object). Without the quest record 0x0B (d2game_objinit.h): 0. With it the object goes to the mode kept as
// a word in the quest data (+0x40) (0xA4A2: object, mode) and the data's word at +0x38 becomes the object's id
// (-1 without an object). A kept mode of 2 also drops the object's collision (0xA550: object), whose answer is
// then the result; otherwise the result is the id just stored.
#pragma once
#include "d2game_objinit.h"

namespace gc32c0 {
template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0xB);
    if (!q) return 0;
    const uint32_t d = e.rd32(q + 0x18);
    const uint32_t mode = e.rd8(d + 0x40) | (e.rd8(d + 0x41) << 8);
    e.h(0xA4A2, 0, 0, 0, 0, 0, e.rd32(op + 4), mode);
    const uint32_t id = gobjinit::ObjId(e, e.rd32(op + 4));
    e.wr32(d + 0x38, id);
    if ((e.rd8(d + 0x40) | (e.rd8(d + 0x41) << 8)) != 2) return id;
    return e.h(0xA550, 0, 0, 0, 0, 0, e.rd32(op + 4));
}
}
