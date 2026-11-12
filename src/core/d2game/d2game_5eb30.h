// 1.13c D2Game+0x5EB30: object init callback of the last act's fifth quest, the cage (table entry). ecx = the
// init block ([0] game, [4] the object, [8] the room, [0x14] x, [0x18] y). With the quest record 0x21
// (d2game_objinit.h) its data keeps the object's id (+0xE4, -1 without an object), 1 in byte +0xE1 and x, y
// (+0xE8, +0xEC). With the data's word +0x84 at 2 the quest sequence step runs (0x56B10: ecx = record) and, with
// byte +0x91 clear, the quest monster is spawned (0x361E0: game, x, y, room, 1, 0x200); it leaves its id at +0x94
// and 1 at +0x91 and, with a map AI list at +0x104 whose word +0x04 is set and byte +0x10C clear, has its map AI
// slot pointed at it (0xA562: ecx = the list's address, edx = the slot), byte +0x10C becoming 1. In every case
// the neighbouring quest is told (0x5E7F0: ecx = game; stack: object), whose answer is the result.
#pragma once
#include "d2game_objinit.h"

namespace g5eb30 {
// the map AI slot of a monster: unit type 1 with data (+0x14): the data's word +0x28 plus 0x38; else 0x38
template <class E> uint32_t MapAi(E& e, uint32_t u)
{
    if (u && e.rd32(u) == 1 && e.rd32(u + 0x14)) return e.rd32(e.rd32(u + 0x14) + 0x28) + 0x38;
    return 0x38;
}

template <class E> uint32_t Init(E& e, uint32_t op)
{
    const uint32_t q = gobjinit::Find(e, op, 0x21);
    if (q) {
        const uint32_t d = e.rd32(q + 0x18);
        e.wr32(d + 0xE4, gobjinit::ObjId(e, e.rd32(op + 4)));
        e.wr8(d + 0xE1, 1);
        e.wr32(d + 0xE8, e.rd32(op + 0x14));
        e.wr32(d + 0xEC, e.rd32(op + 0x18));
        if (e.rd32(d + 0x84) == 2) {
            e.h(0x56B10, 0, q);
            if (!e.rd8(d + 0x91)) {
                const uint32_t u = e.h(0x361E0, 0, 0, 0, 0, 0, e.rd32(op), e.rd32(op + 0x14), e.rd32(op + 0x18), e.rd32(op + 8), 1, 0x200);
                if (u) {
                    const uint32_t list = e.rd32(d + 0x104);
                    e.wr32(d + 0x94, e.rd32(u + 0xC));
                    e.wr8(d + 0x91, 1);
                    if (list && e.rd32(list + 4) && !e.rd8(d + 0x10C)) {
                        e.h(0xA562, 0, d + 0x104, MapAi(e, u));
                        e.wr8(d + 0x10C, 1);
                    }
                }
            }
        }
    }
    return e.h(0x5E7F0, 0, e.rd32(op), 0, 0, 0, e.rd32(op + 4));
}
}
